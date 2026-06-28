// 2026-06-14

////////////////////////////
//- Third-Party Includes

# define XXH_INLINE_ALL
# define XXH_IMPLEMENTATION
# define XXH_STATIC_LINKING_ONLY
#include "third_party/xxhash/xxhash.h"

////////////////////////////
//- Helpers

static inline Vector2 WorldToScreen(Vector2 v)
{
  return (Vector2){ v.x, -v.y };
}

static inline f32 RandF32(f32 a, f32 b)
{
  return a + (f32)rand() / (f32)(RAND_MAX) * (b - a);
}

static inline u8 RandU8(u8 a, u8 b)
{
  return a + rand() % (b - a + 1);
}

// https://www.raylib.com/examples/text/loader.html?name=text_inline_styling
static Vector2 MeasureTextStyled(Font font, const char *text, float fontSize, float spacing)
{
  Vector2 textSize = { 0 };

  if ((font.texture.id == 0) || (text == NULL) || (text[0] == '\0')) return textSize; // Security check

  int textLen = TextLength(text); // Get size in bytes of text
                                  //float textLineSpacing = fontSize*1.5f; // Not used...

  float textWidth = 0.0f;
  float textHeight = fontSize;
  float scaleFactor = fontSize/(float)font.baseSize;

  int codepoint = 0;              // Current character
  int index = 0;                  // Index position in sprite font
  int validCodepointCounter = 0;

  for (int i = 0; i < textLen;)
  {
    int codepointByteCount = 0;
    codepoint = GetCodepointNext(&text[i], &codepointByteCount);

    if (codepoint == '[') // Ignore pipe inline styling
    {
      if (((i + 2) < textLen) && (text[i + 1] == 'r') && (text[i + 2] == ']')) // Reset styling
      {
        i += 3;     // Skip "[r]"
        continue;   // Do not measure characters
      }
      else if (((i + 1) < textLen) && ((text[i + 1] == 'c') || (text[i + 1] == 'b')))
      {
        i += 2;     // Skip "[c" or "[b" to start parsing color

        const char *textPtr = &text[i]; // Color should start here, let's see...

        int colHexCount = 0;
        while ((textPtr != NULL) && (textPtr[colHexCount] != '\0') && (textPtr[colHexCount] != ']'))
        {
          if (((textPtr[colHexCount] >= '0') && (textPtr[colHexCount] <= '9')) ||
              ((textPtr[colHexCount] >= 'A') && (textPtr[colHexCount] <= 'F')) ||
              ((textPtr[colHexCount] >= 'a') && (textPtr[colHexCount] <= 'f')))
          {
            colHexCount++;
          }
          else break; // Only affects while loop
        }

        i += (colHexCount + 1); // Skip color value retrieved and ']'
        continue;   // Do not measure characters
      }
    }
    else if (codepoint != '\n')
    {
      index = GetGlyphIndex(font, codepoint);

      if (font.glyphs[index].advanceX > 0) textWidth += font.glyphs[index].advanceX;
      else textWidth += (font.recs[index].width + font.glyphs[index].offsetX);

      validCodepointCounter++;
      i += codepointByteCount;
    }
  }

  textSize.x = textWidth*scaleFactor + (validCodepointCounter - 1)*spacing;
  textSize.y = textHeight;

  return textSize;
}

////////////////////////////
//- String Helpers

static u32 CstringLength(char *cstr)
{
  u32 len = 0;
  if(cstr)
  {
    char *p = cstr;
    for(; *p!='\0'; p+=1);
    len = p - cstr;
  }
  return len;
}
