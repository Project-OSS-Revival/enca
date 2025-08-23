/*
  Extremely Naive Charset Analyser.  main module

  Copyright (C) 2000-2002 David Necas (Yeti) <yeti@physics.muni.cz>

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as published
  by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/
#include <math.h>
#include "common.h"

/* Local prototypes. */
static int  process_file        (EncaAnalyser an,
                                 const char *fname);
static int  process_mixed_file  (EncaAnalyser an,
                                 const char *fname);
static void dwim_libenca_options(EncaAnalyser an,
                                 const File *file);
static int print_results       (const char *fname,
                                 EncaAnalyser an,
                                 EncaEncoding result,
                                 int gerrno);
static void indent_surface      (const char *s);
static void double_utf8_chk     (EncaAnalyser an,
                                 const unsigned char *sample,
                                 size_t size);

/* process options and do some other initializations, then go through the
   file list and process files one by one
   at the end, exit and return 0 on succes, 1 on failure, 2 on troubles */
int
main(int argc, char *argv[])
{
  char **pp_file, **flist; /* filename list pointer */
  long int err=0; /* nonzero if process_file() ever returned nonzero */
  EncaAnalyser an;

  /* Process command line arguments. */
  pp_file = flist = process_opt(argc, argv);

  /* Initialization. */
  if (options.verbosity_level > 2)
    fprintf(stderr, "Initializing language %s\n", options.language);
  an = enca_analyser_alloc(options.language);
  if (!an) {
    fprintf(stderr, "%s: Language `%s' is unknown or not supported.\n"
                    "Run `%s --list languages' to get list "
                    "of supported languages.\n"
                    "Run `%s -L none' to test only language independent, "
                    "multibyte encodings.\n",
                    program_name, options.language,
                    program_name,
                    program_name);
    exit(EXIT_TROUBLE);
  }

  enca_set_threshold(an, 1.38);
  enca_set_multibyte(an, 1);
  enca_set_ambiguity(an, 1);
  enca_set_garbage_test(an, 1);

  /* Any files specified on command line? */
  if (pp_file == NULL) {
    /* No => read stdin. */
    err = process_file(an, NULL);
  }
  else {
    /* Process file list, cumultate the worst error in err. */
    while (*pp_file != NULL) {
      err |= process_file(an, *pp_file);
      enca_free(*pp_file);
      pp_file++;
    }
  }

  /* Free buffer */
  process_file(NULL, NULL);
  enca_analyser_free(an);
  enca_free(options.language);
  enca_free(options.target_enc_str);
  enca_free(flist);

  if (err & EXIT_TROUBLE)
    err = EXIT_TROUBLE;

  return err;
}

/* process file named fname
   this is the `boss' function
   returns 0 on succes, 1 on failure, 2 on troubles */
static int
process_file(EncaAnalyser an,
             const char *fname)
{
  static int utf8 = ENCA_CS_UNKNOWN;
  static Buffer *buffer = NULL; /* persistent i/o buffer */
  int ot_is_convert = (options.output_type == OTYPE_CONVERT);
  int res = ERR_OK;

  EncaEncoding result; /* the guessed encoding */
  File *file; /* the processed file */

  if (!an) {
    buffer_free(buffer);
    return 0;
  }

  /* Initialize when we are called the first time. */
  if (buffer == NULL)
    buffer = buffer_new(buffer_size);

  if (!enca_charset_is_known(utf8)) {
    utf8 = enca_name_to_charset("utf8");
    assert(enca_charset_is_known(utf8));
  }

  /* Read sample. */
  file = file_new(fname, buffer);
  if (file_open(file, ot_is_convert ? "r+b" : "rb") != 0) {
    file_free(file);
    return EXIT_TROUBLE;
  }
  if (file_read(file) == -1) {
    file_free(file);
    return EXIT_TROUBLE;
  }
  if (!ot_is_convert)
    file_close(file);

  /* Check if mixed encoding processing is requested */
  if (options.mixed_encodings) {
    file_close(file);
    file_free(file);
    return process_mixed_file(an, fname);
  }

  /* Guess encoding. */
  dwim_libenca_options(an, file);
  if (ot_is_convert)
    result = enca_analyse_const(an, buffer->data, buffer->pos);
  else
    result = enca_analyse(an, buffer->data, buffer->pos);

  /* Is conversion required? */
  if (ot_is_convert) {
    int err = 0;

    if (enca_charset_is_known(result.charset))
      err = convert(file, result);
    else {
      if (enca_errno(an) != ENCA_EEMPTY) {
        fprintf(stderr, "%s: Cannot convert `%s' from unknown encoding\n",
                        program_name,
                        ffname_r(file->name));
      }
      /* Copy stdin to stdout unchanged. */
      if (file->name == NULL)
        err = copy_and_convert(file, file, NULL);
    }

    file_free(file);
    if ((err == ERR_OK && !enca_charset_is_known(result.charset)
         && enca_errno(an) != ENCA_EEMPTY)
        || err == ERR_CANNOT)
      return EXIT_FAILURE;

    return (err == ERR_OK) ? EXIT_SUCCESS : EXIT_TROUBLE;
  }

  /* Print results. */
  res = print_results(file->name, an, result, enca_errno(an));
  if (result.charset == utf8)
    double_utf8_chk(an, buffer->data, buffer->pos);

  file_free(file);
  return (res == ERR_OK) && enca_charset_is_known(result.charset) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 * DWIM
 *
 * Choose some suitable values of all the libenca tuning parameters.
 */
static void
dwim_libenca_options(EncaAnalyser an, const File *file)
{
  const double mu = 0.005;  /* derivation in 0 */
  const double m = 15.0;  /* value in infinity */
  ssize_t size = file->buffer->pos;
  size_t sgnf;

  /* The number of significant characters */
  if (!size)
    sgnf = 1;
  else
    sgnf = ceil((double)size/(size/m + 1.0/mu));
  enca_set_significant(an, sgnf);

  /* When buffer contains whole file, require correct termination. */
  if (file->size == size)
    enca_set_termination_strictness(an, 1);
  else
    enca_set_termination_strictness(an, 0);

  enca_set_filtering(an, sgnf > 2);
}

/**
 * Prints results.
 **/
static int
print_results(const char *fname,
              EncaAnalyser an,
              EncaEncoding result,
              int gerrno)
{
  char *s;
  int res = ERR_OK;
  EncaSurface surf = result.surface
                     & ~enca_charset_natural_surface(result.charset);

  if (options.prefix_filename)
    printf("%s: ", ffname_r(fname));

  switch (options.output_type) {
    case OTYPE_ALIASES:
    print_aliases(result.charset);
    break;

    case OTYPE_CANON:
    if (surf) {
      s = enca_get_surface_name(surf, ENCA_NAME_STYLE_ENCA);
      fputs(enca_charset_name(result.charset, ENCA_NAME_STYLE_ENCA), stdout);
      puts(s);
      enca_free(s);
    }
    else
      puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_ENCA));
    break;

    case OTYPE_HUMAN:
    case OTYPE_DETAILS:
    if (surf) {
      s = enca_get_surface_name(surf, ENCA_NAME_STYLE_HUMAN);
      puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_HUMAN));
      indent_surface(s);
      enca_free(s);
    }
    else
      puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_HUMAN));
    break;

    case OTYPE_RFC1345:
    puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_RFC1345));
    break;

    case OTYPE_CS2CS:
    if (enca_charset_name(result.charset, ENCA_NAME_STYLE_CSTOCS) != NULL)
      puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_CSTOCS));
    else {
      puts(enca_charset_name(ENCA_CS_UNKNOWN, ENCA_NAME_STYLE_CSTOCS));
      res = ERR_CANNOT;
    }
    break;

    case OTYPE_ICONV:
    if (enca_charset_name(result.charset, ENCA_NAME_STYLE_ICONV) != NULL)
      puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_ICONV));
    else {
      puts(enca_charset_name(ENCA_CS_UNKNOWN, ENCA_NAME_STYLE_ICONV));
      res = ERR_CANNOT;
    }
    break;

    case OTYPE_MIME:
    if (enca_charset_name(result.charset, ENCA_NAME_STYLE_MIME) != NULL)
      puts(enca_charset_name(result.charset, ENCA_NAME_STYLE_MIME));
    else {
      puts(enca_charset_name(ENCA_CS_UNKNOWN, ENCA_NAME_STYLE_MIME));
      res = ERR_CANNOT;
    }
    break;

    default:
    abort();
    break;
  }

  if (gerrno && options.output_type == OTYPE_DETAILS) {
    printf("  Failure reason: %s.\n", enca_strerror(an, gerrno));
    res = ERR_CANNOT;
  }
  return res;
}

/**
 * Reformats surface names as returned from enca_get_surface_name() one
 * per line to be indented and prints them.
 **/
static void
indent_surface(const char *s)
{
  const char *p;

  while ((p = strchr(s, '\n')) != NULL) {
    p++;
    printf("  %.*s", (int)(p-s), s);
    s = p;
  }
}

/**
 * Checks for doubly-encoded UTF-8 and prints a line when it looks so.
 **/
static void
double_utf8_chk(EncaAnalyser an,
                const unsigned char *sample,
                size_t size)
{
  size_t dbl, i;
  int *candidates;

  if (options.output_type != OTYPE_DETAILS
      && options.output_type != OTYPE_HUMAN)
    return;

  dbl = enca_double_utf8_check(an, sample, size);
  if (!dbl)
    return;

  candidates = enca_double_utf8_get_candidates(an);
  if (candidates == NULL)
    return;
  if (dbl == 1)
    printf("  Doubly-encoded to UTF-8 from");
  else
    printf("  Doubly-encoded to UTF-8 from one of:");

  for (i = 0; i < dbl; i++)
    printf(" %s", enca_charset_name(candidates[i], ENCA_NAME_STYLE_ENCA));

  putchar('\n');
  enca_free(candidates);
}

/**
 * Structure to hold information about a detected encoding segment
 */
typedef struct {
  size_t start;          /* Start position in file */
  size_t length;         /* Length of this segment */
  EncaEncoding encoding; /* Detected encoding for this segment */
} EncodingSegment;

/**
 * Process a file with potentially mixed encodings by analyzing it in chunks
 * and handling conversions segment by segment.
 */
static int
process_mixed_file(EncaAnalyser an, const char *fname)
{
  static Buffer *buffer = NULL;
  static int utf8 = ENCA_CS_UNKNOWN;
  FILE *infile = NULL;
  FILE *outfile = NULL;
  char *temp_filename = NULL;
  int ot_is_convert = (options.output_type == OTYPE_CONVERT);
  int res = ERR_OK;
  
  const size_t CHUNK_SIZE = 1024; /* Process file in 1KB chunks */
  
  EncodingSegment *segments = NULL;
  size_t segment_count = 0;
  size_t segment_capacity = 8;
  
  unsigned char *chunk_buffer = NULL;
  size_t file_pos = 0;

  if (buffer == NULL)
    buffer = buffer_new(buffer_size);

  if (!enca_charset_is_known(utf8)) {
    utf8 = enca_name_to_charset("utf8");
    assert(enca_charset_is_known(utf8));
  }

  /* Open input file */
  if (fname == NULL) {
    infile = stdin;
  } else {
    infile = fopen(fname, "rb");
    if (infile == NULL) {
      fprintf(stderr, "%s: Cannot open file %s: %s\n", 
              program_name, fname, strerror(errno));
      return EXIT_TROUBLE;
    }
  }

  /* Allocate segment array and chunk buffer */
  segments = NEW(EncodingSegment, segment_capacity);
  chunk_buffer = NEW(unsigned char, CHUNK_SIZE);
  
  if (!segments || !chunk_buffer) {
    fprintf(stderr, "%s: Memory allocation failed\n", program_name);
    res = EXIT_TROUBLE;
    goto cleanup;
  }

  if (options.verbosity_level > 1) {
    fprintf(stderr, "Processing file with mixed encodings in %zu-byte chunks\n", CHUNK_SIZE);
  }

  /* Process file chunk by chunk */
  while (!feof(infile)) {
    size_t bytes_read;
    EncaEncoding detected;
    
    /* Read chunk */
    bytes_read = fread(chunk_buffer, 1, CHUNK_SIZE, infile);
    if (bytes_read == 0) break;
    
    /* Detect encoding for this chunk */
    detected = enca_analyse_const(an, chunk_buffer, bytes_read);

    /* Expand segments array if needed */
    if (segment_count >= segment_capacity) {
      segment_capacity *= 2;
      segments = realloc(segments, segment_capacity * sizeof(EncodingSegment));
      if (!segments) {
        fprintf(stderr, "%s: Memory reallocation failed\n", program_name);
        res = EXIT_TROUBLE;
        goto cleanup;
      }
    }

    /* Add segment or merge with previous if same encoding */
    if (segment_count > 0 && 
        segments[segment_count - 1].encoding.charset == detected.charset &&
        segments[segment_count - 1].encoding.surface == detected.surface) {
      /* Merge with previous segment */
      segments[segment_count - 1].length += bytes_read;
    } else {
      /* Create new segment */
      segments[segment_count].start = file_pos;
      segments[segment_count].length = bytes_read;
      segments[segment_count].encoding = detected;
      segment_count++;
    }

    file_pos += bytes_read;
  }

  /* Print detection results */
  if (!ot_is_convert) {
    if (options.prefix_filename && fname != NULL) {
      printf("%s: ", fname);
    }
    
    if (segment_count == 0) {
      printf("No data processed\n");
    } else if (segment_count == 1) {
      /* Only one encoding found */
      if (enca_charset_is_known(segments[0].encoding.charset)) {
        print_results(fname, an, segments[0].encoding, enca_errno(an));
      } else {
        printf("Single segment with unrecognized encoding\n");
      }
    } else {
      /* Multiple encodings found */
      printf("Mixed encodings detected (%zu segments):\n", segment_count);
      for (size_t i = 0; i < segment_count; i++) {
        printf("  Segment %zu (offset %zu, %zu bytes): ", 
               i + 1, segments[i].start, segments[i].length);
        
        if (enca_charset_is_known(segments[i].encoding.charset)) {
          const char *enc_name = enca_charset_name(segments[i].encoding.charset, 
                                                  ENCA_NAME_STYLE_HUMAN);
          if (enc_name != NULL) {
            printf("%s", enc_name);
          } else {
            printf("Known but unnamed charset %d", segments[i].encoding.charset);
          }
          
          if (segments[i].encoding.surface) {
            const char *surface_name = enca_get_surface_name(segments[i].encoding.surface, 
                                                            ENCA_NAME_STYLE_HUMAN);
            if (surface_name) {
              printf(" (%s)", surface_name);
            }
          }
        } else {
          printf("Unrecognized encoding");
        }
        printf("\n");
      }
    }
  }

  /* Handle conversion if requested */
  if (ot_is_convert && enca_charset_is_known(options.target_enc.charset)) {
    fprintf(stderr, "Mixed encoding conversion not fully implemented yet.\n");
    fprintf(stderr, "Would convert %zu segments to %s\n", 
            segment_count, options.target_enc_str);
  }

cleanup:
  if (infile != NULL && infile != stdin) {
    fclose(infile);
  }
  if (outfile != NULL && outfile != stdout) {
    fclose(outfile);
  }
  if (temp_filename != NULL) {
    unlink(temp_filename); /* Remove temp file on error */
    enca_free(temp_filename);
  }
  if (segments != NULL) {
    enca_free(segments);
  }
  if (chunk_buffer != NULL) {
    enca_free(chunk_buffer);
  }

  return res;
}

/* vim: ts=2
 */
