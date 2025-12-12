#include "cor_complementar.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



static int hex_value(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return 10 + (c - 'a');
  if (c >= 'A' && c <= 'F')
    return 10 + (c - 'A');
  return -1;
}


static int parse_hex_color( char *s, int *r, int *g, int *b) {
  if (s == NULL || s[0] != '#')
    return -1;
  if (strlen(s) != 7)
    return -1;
  int v[6];
  for (int i = 0; i < 6; i++) {
    int hv = hex_value(s[1 + i]);
    if (hv < 0)
      return -1;
    v[i] = hv;
  }
  *r = v[0] * 16 + v[1];
  *g = v[2] * 16 + v[3];
  *b = v[4] * 16 + v[5];
  return 0;
}

static int cor_nomeada_rgb( char *name, int *r, int *g, int *b) {
  if (name == NULL)
    return -1;
 
  if (strcmp(name, "black") == 0) {
    *r = 0;
    *g = 0;
    *b = 0;
    return 0;
  } else if (strcmp(name, "white") == 0) {
    *r = 255;
    *g = 255;
    *b = 255;
    return 0;
  } else if (strcmp(name, "red") == 0) {
    *r = 255;
    *g = 0;
    *b = 0;
    return 0;
  } else if (strcmp(name, "green") == 0) {
    *r = 0;
    *g = 128;
    *b = 0;
    return 0;
  } else if (strcmp(name, "blue") == 0) {
    *r = 0;
    *g = 0;
    *b = 255;
    return 0;
  } else if (strcmp(name, "yellow") == 0) {
    *r = 255;
    *g = 255;
    *b = 0;
    return 0;
  } else if (strcmp(name, "pink") == 0) {
    *r = 255;
    *g = 192;
    *b = 203;
    return 0;
  } else if (strcmp(name, "cyan") == 0) {
    *r = 0;
    *g = 255;
    *b = 255;
    return 0;
  } else if (strcmp(name, "orange") == 0) {
    *r = 255;
    *g = 165;
    *b = 0;
    return 0;
  } else if (strcmp(name, "teal") == 0) {
    *r = 0;
    *g = 128;
    *b = 128;
    return 0;
  } else if (strcmp(name, "purple") == 0) {
    *r = 128;
    *g = 0;
    *b = 128;
    return 0;
  }
  return -1;
}

char *cor_complementar( char *cor) {
  if (cor == NULL)
    return NULL;
  int r = 0, g = 0, b = 0;
  int ok = parse_hex_color(cor, &r, &g, &b);
  if (ok != 0) {
    if (cor_nomeada_rgb(cor, &r, &g, &b) != 0) {
      
      

      return cor;
    }
  }

  int ir = 255 - r;
  int ig = 255 - g;
  int ib = 255 - b;

  char *out = (char *)malloc(8);
  if (out == NULL)
    return NULL;
  
  int n = snprintf(out, 8, "#%02X%02X%02X", ir, ig, ib);
  if (n < 0 || n >= 8) {
    free(out);
    return NULL;
  }
  return out;
}