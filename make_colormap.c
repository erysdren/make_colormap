
/*
 * quake colormap generator
 *
 * authors: metslime, erysdren
 * https://quakewiki.org/wiki/Quake_palette
 *
 * license: public domain
 */

#include <stdio.h>
#include <stdlib.h>

unsigned char convert_24_to_8(const unsigned char palette[768], const int rgb[3])
{
  int i, j;
  int best_index = -1;
  int best_dist = 0;

  for (i = 0; i < 256; i++)
  {
    int dist = 0;

    for (j = 0; j < 3; j++)
    {
    /* note that we could use RGB luminosity bias for greater accuracy, but quake's colormap apparently didn't do this */
      int d = abs(rgb[j] - palette[i*3+j]);
      dist += d * d;
    }

    if (best_index == -1 || dist < best_dist)
    {
      best_index = i;
      best_dist = dist;
    }
  }

  return (unsigned char)best_index;
}

void generate_colormap(const unsigned char palette[768], unsigned char out_colormap[16384])
{
  int num_fullbrights = 32; /* the last 32 colours will be full bright */
  int x, y, i;

  for (x = 0; x < 256; x++)
  {
    for (y = 0; y < 64; y++)
    {
      if (x < 256 - num_fullbrights)
      {
        int rgb[3];

        for (i = 0; i < 3; i++)
        {
          rgb[i] = (palette[x*3+i] * (63 - y) + 16) >> 5; /* divide by 32, rounding to nearest integer */
          if (rgb[i] > 255)
            rgb[i] = 255;
        }

        out_colormap[y*256+x] = convert_24_to_8(palette, rgb);
      }
      else
      {
      /* this colour is a fullbright, just keep the original colour */
        out_colormap[y*256+x] = x;
      }
    }
  }
}

int main(int argc, char **argv)
{
    FILE *palettefile, *colormapfile;
    unsigned char palette[768];
    unsigned char colormap[16384];

    palettefile = fopen("palette.lmp", "rb");
    if (!palettefile)
    {
      printf("error: couldn't open palette.lmp for reading\n");
      return 1;
    }

    colormapfile = fopen("colormap.lmp", "wb");
    if (!colormapfile)
    {
      printf("error: couldn't open colormap.lmp for writing\n");
      return 1;
    }

    fread(palette, 768, 1, palettefile);
    generate_colormap(palette, colormap);
    fwrite(colormap, 16384, 1, colormapfile);

    fclose(palettefile);
    fclose(colormapfile);

    return 0;
}

