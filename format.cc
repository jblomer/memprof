#define __STDC_FORMAT_MACROS

#include <errno.h>
#include <inttypes.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

static void Usage(char *prog) {
  printf("%s file1 [file2 ...] | gnuplot\n", prog);
}

static bool GetLineFile(FILE *f, std::string *line) {
  int retval;
  line->clear();
  while (true) {
    retval = fgetc(f);
    if (ferror(f) && (errno == EINTR)) {
      clearerr(f);
      continue;
    } else if (retval == EOF) {break;}
    char c = retval;
    if (c == '\n')
      break;
    line->push_back(c);
  }
  return (retval != EOF) || !line->empty();
}

static uint64_t String2Uint64(const string &value) {
  uint64_t result;
  sscanf(value.c_str(), "%"PRIu64, &result);
  return result;
}


int main(int argc, char **argv) {
  if (argc < 2) {
    Usage(argv[0]);
    return 1;
  }

  unsigned nfiles = argc - 1;

  vector<unsigned> *profiles = new vector<unsigned>[nfiles];
  vector<string> names;

  unsigned max_entries = 0;
  for (unsigned i = 0; i < nfiles; ++i) {
    string filename = argv[i + 1];
    names.push_back(filename);
    FILE *f = fopen(filename.c_str(), "r");
    assert(f != NULL);
    string line;
    while (GetLineFile(f, &line)) {
      if (line[0] == '#')
        continue;
      profiles[i].push_back(String2Uint64(line));
    }
    fprintf(stderr, "File %s: %lu values\n",
            filename.c_str(), profiles[i].size());
    if (profiles[i].size() > max_entries)
      max_entries = profiles[i].size();
  }

  printf("set term dumb size 160,50 aspect 1\n");
  printf("set xlabel \"Time\"\n");
  printf("set ylabel \"RSS [kB]\"\n");
  printf("plot '-' ");

  for (unsigned i = 0; i < nfiles; ++i) {
    if (i > 0)
      printf(", '' ");
    printf("using 1 title \"%s\"", names[i].c_str());
  }
  printf(";\n");

  for (unsigned j_file = 0; j_file < nfiles; ++j_file) {
    for (unsigned i_entry = 0; i_entry < max_entries; ++i_entry) {
      double progress = double(i_entry) / double(max_entries);
      assert(0.0 <= progress);
      assert(1.0 >= progress);
      double bin = profiles[j_file].size() * progress;
      double progress_in_bin = bin - floor(bin);
      int bin_lower = floor(bin);
      int bin_upper = ceil(bin);
      if (bin_upper >= profiles[j_file].size())
        bin_upper = bin_lower;
      double val_lower = profiles[j_file][bin_lower];
      double val_upper = profiles[j_file][bin_upper];
      double value = val_lower + (val_upper - val_lower) * progress_in_bin;
      printf("%u\n", unsigned(round(value)));
    }
    printf("e\n");
  }

  return 0;
}
