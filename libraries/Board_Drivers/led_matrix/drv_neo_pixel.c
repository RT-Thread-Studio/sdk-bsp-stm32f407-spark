/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-09-20     yuanjie      port Adafruit neopixel.
 */

#include <board.h>
#include <drv_neo_pixel.h>

#if defined(BSP_USING_LED_MATRIX) && defined(BSP_USING_NEOPIXEL)

// bool begun;         ///< true if begin() previously called
uint16_t numLEDs;  ///< Number of RGB LEDs in strip
uint16_t numBytes; ///< Size of 'pixels' buffer below
// int16_t pin;        ///< Output pin number (-1 if not yet set)
uint8_t brightness; ///< Strip brightness 0-255 (stored as +1)
uint8_t *pixels;    ///< Holds LED color values (3 or 4 bytes each)
uint8_t rOffset;    ///< Red index within each 3- or 4-byte pixel
uint8_t gOffset;    ///< Index of green byte
uint8_t bOffset;    ///< Index of blue byte
uint8_t wOffset;    ///< Index of white (==rOffset if no white)
// uint32_t endTime;   ///< Latch timing reference

/* A PROGMEM (flash mem) table containing 8-bit unsigned sine wave (0-255).
   Copy & paste this snippet into a Python REPL to regenerate:
import math
for x in range(256):
    print("{:3},".format(int((math.sin(x/128.0*math.pi)+1.0)*127.5+0.5))),
    if x&15 == 15: print
*/
static const uint8_t _NeoPixelSineTable[256] = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170,
    173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211,
    213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240,
    241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254,
    254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251,
    250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 238, 237, 235, 234, 232,
    230, 228, 226, 224, 222, 220, 218, 215, 213, 211, 208, 206, 203, 201, 198,
    196, 193, 190, 188, 185, 182, 179, 176, 173, 170, 167, 165, 162, 158, 155,
    152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109,
    106, 103, 100, 97, 93, 90, 88, 85, 82, 79, 76, 73, 70, 67, 65,
    62, 59, 57, 54, 52, 49, 47, 44, 42, 40, 37, 35, 33, 31, 29,
    27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11, 10, 9, 7, 6,
    5, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9, 10, 11,
    12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35, 37,
    40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76,
    79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115, 118, 121,
    124};

/* Similar to above, but for an 8-bit gamma-correction table.
   Copy & paste this snippet into a Python REPL to regenerate:
import math
gamma=2.6
for x in range(256):
    print("{:3},".format(int(math.pow((x)/255.0,gamma)*255.0+0.5))),
    if x&15 == 15: print
*/
static const uint8_t _NeoPixelGammaTable[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6,
    6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10,
    11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17,
    17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35,
    36, 37, 38, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 68, 69, 70, 71, 72, 73, 75, 76, 77, 78, 80, 81,
    82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96, 97, 99, 100, 102,
    103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 124, 125,
    127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148, 150, 152,
    154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
    184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252,
    255};

/**
@brief   An 8-bit integer sine wave function, not directly compatible
            with standard trigonometric units like radians or degrees.
@param   x  Input angle, 0-255; 256 would loop back to zero, completing
            the circle (equivalent to 360 degrees or 2 pi radians).
            One can therefore use an unsigned 8-bit variable and simply
            add or subtract, allowing it to overflow/underflow and it
            still does the expected contiguous thing.
@return  Sine result, 0 to 255, or -128 to +127 if type-converted to
            a signed int8_t, but you'll most likely want unsigned as this
            output is often used for pixel brightness in animation effects.
*/
static uint8_t sine8(uint8_t x)
{
    return _NeoPixelSineTable[x]; // 0-255 in, 0-255 out
}

/**
@brief   An 8-bit gamma-correction function for basic pixel brightness
            adjustment. Makes color transitions appear more perceptially
            correct.
@param   x  Input brightness, 0 (minimum or off/black) to 255 (maximum).
@return  Gamma-adjusted brightness, can then be passed to one of the
            setPixelColor() functions. This uses a fixed gamma correction
            exponent of 2.6, which seems reasonably okay for average
            NeoPixels in average tasks. If you need finer control you'll
            need to provide your own gamma-correction function instead.
*/
static uint8_t gamma8(uint8_t x)
{
    return _NeoPixelGammaTable[x]; // 0-255 in, 0-255 out
}

/**
  @brief   Change the length of a previously-declared Adafruit_NeoPixel
           strip object. Old data is deallocated and new data is cleared.
           Pin number and pixel format are unchanged.
  @param   n  New length of strip, in pixels.
  @note    This function is deprecated, here only for old projects that
           may still be calling it. New projects should instead use the
           'new' keyword with the first constructor syntax (length, pin,
           type).
*/
void updateLength(uint16_t n)
{
    rt_free(pixels); // Free existing data (if any)

    // Allocate new data -- note: ALL PIXELS ARE CLEARED
    numBytes = n * ((wOffset == rOffset) ? 3 : 4);
    if ((pixels = (uint8_t *)rt_malloc(numBytes)))
    {
        rt_memset(pixels, 0, numBytes);
        numLEDs = n;
    }
    else
    {
        numLEDs = numBytes = 0;
    }
}

/**
  @brief   Set a pixel's color using a 32-bit 'packed' RGB or RGBW value.
  @param   n  Pixel index, starting from 0.
  @param   c  32-bit color value. Most significant byte is white (for RGBW
              pixels) or ignored (for RGB pixels), next is red, then green,
              and least significant byte is blue.
*/
void setPixelColor(uint16_t n, uint32_t c)
{
    if (n < numLEDs)
    {
        uint8_t *p, r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c;
        if (brightness)
        { // See notes in setBrightness()
            r = (r * brightness) >> 8;
            g = (g * brightness) >> 8;
            b = (b * brightness) >> 8;
        }
        if (wOffset == rOffset)
        {
            p = &pixels[n * 3];
        }
        else
        {
            p = &pixels[n * 4];
            uint8_t w = (uint8_t)(c >> 24);
            p[wOffset] = brightness ? ((w * brightness) >> 8) : w;
        }
        p[rOffset] = r;
        p[gOffset] = g;
        p[bOffset] = b;
    }
}

/**
  @brief   Set a pixel's color using separate red, green and blue
           components. If using RGBW pixels, white will be set to 0.
  @param   n  Pixel index, starting from 0.
  @param   r  Red brightness, 0 = minimum (off), 255 = maximum.
  @param   g  Green brightness, 0 = minimum (off), 255 = maximum.
  @param   b  Blue brightness, 0 = minimum (off), 255 = maximum.
*/
void setPixelColorRGB(uint16_t n, uint8_t r, uint8_t g,
                      uint8_t b)
{

    if (n < numLEDs)
    {
        if (brightness)
        { // See notes in setBrightness()
            r = (r * brightness) >> 8;
            g = (g * brightness) >> 8;
            b = (b * brightness) >> 8;
        }
        uint8_t *p;
        if (wOffset == rOffset)
        {                       // Is an RGB-type strip
            p = &pixels[n * 3]; // 3 bytes per pixel
        }
        else
        {                       // Is a WRGB-type strip
            p = &pixels[n * 4]; // 4 bytes per pixel
            p[wOffset] = 0;     // But only R,G,B passed -- set W to 0
        }
        p[rOffset] = r; // R,G,B always stored
        p[gOffset] = g;
        p[bOffset] = b;
    }
}

/**
  @brief   Set a pixel's color using separate red, green, blue and white
           components (for RGBW NeoPixels only).
  @param   n  Pixel index, starting from 0.
  @param   r  Red brightness, 0 = minimum (off), 255 = maximum.
  @param   g  Green brightness, 0 = minimum (off), 255 = maximum.
  @param   b  Blue brightness, 0 = minimum (off), 255 = maximum.
  @param   w  White brightness, 0 = minimum (off), 255 = maximum, ignored
              if using RGB pixels.
*/
void setPixelColorRGBW(uint16_t n, uint8_t r, uint8_t g,
                       uint8_t b, uint8_t w)
{

    if (n < numLEDs)
    {
        if (brightness)
        { // See notes in setBrightness()
            r = (r * brightness) >> 8;
            g = (g * brightness) >> 8;
            b = (b * brightness) >> 8;
            w = (w * brightness) >> 8;
        }
        uint8_t *p;
        if (wOffset == rOffset)
        {                       // Is an RGB-type strip
            p = &pixels[n * 3]; // 3 bytes per pixel (ignore W)
        }
        else
        {                       // Is a WRGB-type strip
            p = &pixels[n * 4]; // 4 bytes per pixel
            p[wOffset] = w;     // Store W
        }
        p[rOffset] = r; // Store R,G,B
        p[gOffset] = g;
        p[bOffset] = b;
    }
}

/**
  @brief   Fill all or part of the NeoPixel strip with a color.
  @param   c      32-bit color value. Most significant byte is white (for
                  RGBW pixels) or ignored (for RGB pixels), next is red,
                  then green, and least significant byte is blue. If all
                  arguments are unspecified, this will be 0 (off).
  @param   first  Index of first pixel to fill, starting from 0. Must be
                  in-bounds, no clipping is performed. 0 if unspecified.
  @param   count  Number of pixels to fill, as a positive value. Passing
                  0 or leaving unspecified will fill to end of strip.
*/
void fill(uint32_t c, uint16_t first, uint16_t count)
{
    uint16_t i, end;

    if (first >= numLEDs)
    {
        return; // If first LED is past end of strip, nothing to do
    }

    // Calculate the index ONE AFTER the last pixel to fill
    if (count == 0)
    {
        // Fill to end of strip
        end = numLEDs;
    }
    else
    {
        // Ensure that the loop won't go past the last pixel
        end = first + count;
        if (end > numLEDs)
            end = numLEDs;
    }

    for (i = first; i < end; i++)
    {
        setPixelColor(i, c);
    }
}

/**
  @brief   Convert hue, saturation and value into a packed 32-bit RGB color
           that can be passed to setPixelColor() or other RGB-compatible
           functions.
  @param   hue  An unsigned 16-bit value, 0 to 65535, representing one full
                loop of the color wheel, which allows 16-bit hues to "roll
                over" while still doing the expected thing (and allowing
                more precision than the wheel() function that was common to
                prior NeoPixel examples).
  @param   sat  Saturation, 8-bit value, 0 (min or pure grayscale) to 255
                (max or pure hue). Default of 255 if unspecified.
  @param   val  Value (brightness), 8-bit value, 0 (min / black / off) to
                255 (max or full brightness). Default of 255 if unspecified.
  @return  Packed 32-bit RGB with the most significant byte set to 0 -- the
           white element of WRGB pixels is NOT utilized. Result is linearly
           but not perceptually correct, so you may want to pass the result
           through the gamma32() function (or your own gamma-correction
           operation) else colors may appear washed out. This is not done
           automatically by this function because coders may desire a more
           refined gamma-correction function than the simplified
           one-size-fits-all operation of gamma32(). Diffusing the LEDs also
           really seems to help when using low-saturation colors.
*/
uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)
{

    uint8_t r, g, b;

    // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
    // 0 is not the start of pure red, but the midpoint...a few values above
    // zero and a few below 65536 all yield pure red (similarly, 32768 is the
    // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
    // each for red, green, blue) really only allows for 1530 distinct hues
    // (not 1536, more on that below), but the full unsigned 16-bit type was
    // chosen for hue so that one's code can easily handle a contiguous color
    // wheel by allowing hue to roll over in either direction.
    hue = (hue * 1530L + 32768) / 65536;
    // Because red is centered on the rollover point (the +32768 above,
    // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
    // where 0 and 1530 would yield the same thing. Rather than apply a
    // costly modulo operator, 1530 is handled as a special case below.

    // So you'd think that the color "hexcone" (the thing that ramps from
    // pure red, to pure yellow, to pure green and so forth back to red,
    // yielding six slices), and with each color component having 256
    // possible values (0-255), might have 1536 possible items (6*256),
    // but in reality there's 1530. This is because the last element in
    // each 256-element slice is equal to the first element of the next
    // slice, and keeping those in there this would create small
    // discontinuities in the color wheel. So the last element of each
    // slice is dropped...we regard only elements 0-254, with item 255
    // being picked up as element 0 of the next slice. Like this:
    // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
    // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
    // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
    // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
    // the constants below are not the multiples of 256 you might expect.

    // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
    if (hue < 510)
    { // Red to Green-1
        b = 0;
        if (hue < 255)
        { //   Red to Yellow-1
            r = 255;
            g = hue; //     g = 0 to 254
        }
        else
        {                  //   Yellow to Green-1
            r = 510 - hue; //     r = 255 to 1
            g = 255;
        }
    }
    else if (hue < 1020)
    { // Green to Blue-1
        r = 0;
        if (hue < 765)
        { //   Green to Cyan-1
            g = 255;
            b = hue - 510; //     b = 0 to 254
        }
        else
        {                   //   Cyan to Blue-1
            g = 1020 - hue; //     g = 255 to 1
            b = 255;
        }
    }
    else if (hue < 1530)
    { // Blue to Red-1
        g = 0;
        if (hue < 1275)
        {                   //   Blue to Magenta-1
            r = hue - 1020; //     r = 0 to 254
            b = 255;
        }
        else
        { //   Magenta to Red-1
            r = 255;
            b = 1530 - hue; //     b = 255 to 1
        }
    }
    else
    { // Last 0.5 Red (quicker than % operator)
        r = 255;
        g = b = 0;
    }

    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat;  // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
           (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
           (((((b * s1) >> 8) + s2) * v1) >> 8);
}

/**
  @brief   Query the color of a previously-set pixel.
  @param   n  Index of pixel to read (0 = first).
  @return  'Packed' 32-bit RGB or WRGB value. Most significant byte is white
           (for RGBW pixels) or 0 (for RGB pixels), next is red, then green,
           and least significant byte is blue.
  @note    If the strip brightness has been changed from the default value
           of 255, the color read from a pixel may not exactly match what
           was previously written with one of the setPixelColor() functions.
           This gets more pronounced at lower brightness levels.
*/
uint32_t getPixelColor(uint16_t n)
{
    if (n >= numLEDs)
        return 0; // Out of bounds, return no color.

    uint8_t *p;

    if (wOffset == rOffset)
    { // Is RGB-type device
        p = &pixels[n * 3];
        if (brightness)
        {
            // Stored color was decimated by setBrightness(). Returned value
            // attempts to scale back to an approximation of the original 24-bit
            // value used when setting the pixel color, but there will always be
            // some error -- those bits are simply gone. Issue is most
            // pronounced at low brightness levels.
            return (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
                   (((uint32_t)(p[gOffset] << 8) / brightness) << 8) |
                   ((uint32_t)(p[bOffset] << 8) / brightness);
        }
        else
        {
            // No brightness adjustment has been made -- return 'raw' color
            return ((uint32_t)p[rOffset] << 16) | ((uint32_t)p[gOffset] << 8) |
                   (uint32_t)p[bOffset];
        }
    }
    else
    { // Is RGBW-type device
        p = &pixels[n * 4];
        if (brightness)
        { // Return scaled color
            return (((uint32_t)(p[wOffset] << 8) / brightness) << 24) |
                   (((uint32_t)(p[rOffset] << 8) / brightness) << 16) |
                   (((uint32_t)(p[gOffset] << 8) / brightness) << 8) |
                   ((uint32_t)(p[bOffset] << 8) / brightness);
        }
        else
        { // Return raw color
            return ((uint32_t)p[wOffset] << 24) | ((uint32_t)p[rOffset] << 16) |
                   ((uint32_t)p[gOffset] << 8) | (uint32_t)p[bOffset];
        }
    }
}

/**
  @brief   Adjust output brightness. Does not immediately affect what's
           currently displayed on the LEDs. The next call to show() will
           refresh the LEDs at this level.
  @param   b  Brightness setting, 0=minimum (off), 255=brightest.
  @note    This was intended for one-time use in one's setup() function,
           not as an animation effect in itself. Because of the way this
           library "pre-multiplies" LED colors in RAM, changing the
           brightness is often a "lossy" operation -- what you write to
           pixels isn't necessary the same as what you'll read back.
           Repeated brightness changes using this function exacerbate the
           problem. Smart programs therefore treat the strip as a
           write-only resource, maintaining their own state to render each
           frame of an animation, not relying on read-modify-write.
*/
void setBrightness(uint8_t b)
{
    // Stored brightness value is different than what's passed.
    // This simplifies the actual scaling math later, allowing a fast
    // 8x8-bit multiply and taking the MSB. 'brightness' is a uint8_t,
    // adding 1 here may (intentionally) roll over...so 0 = max brightness
    // (color values are interpreted literally; no scaling), 1 = min
    // brightness (off), 255 = just below max brightness.
    uint8_t newBrightness = b + 1;
    if (newBrightness != brightness)
    { // Compare against prior value
        // Brightness has changed -- re-scale existing data in RAM,
        // This process is potentially "lossy," especially when increasing
        // brightness. The tight timing in the WS2811/WS2812 code means there
        // aren't enough free cycles to perform this scaling on the fly as data
        // is issued. So we make a pass through the existing color data in RAM
        // and scale it (subsequent graphics commands also work at this
        // brightness level). If there's a significant step up in brightness,
        // the limited number of steps (quantization) in the old data will be
        // quite visible in the re-scaled version. For a non-destructive
        // change, you'll need to re-render the full strip data. C'est la vie.
        uint8_t c, *ptr = pixels,
                   oldBrightness = brightness - 1; // De-wrap old brightness value
        uint16_t scale;
        if (oldBrightness == 0)
            scale = 0; // Avoid /0
        else if (b == 255)
            scale = 65535 / oldBrightness;
        else
            scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
        for (uint16_t i = 0; i < numBytes; i++)
        {
            c = *ptr;
            *ptr++ = (c * scale) >> 8;
        }
        brightness = newBrightness;
    }
}

/*!
  @brief   Retrieve the last-set brightness value for the strip.
  @return  Brightness value: 0 = minimum (off), 255 = maximum.
*/
uint8_t getBrightness(void) { return brightness - 1; }

/*!
  @brief   Fill the whole NeoPixel strip with 0 / black / off.
*/
void clear(void) { memset(pixels, 0, numBytes); }

// A 32-bit variant of gamma8() that applies the same function
// to all components of a packed RGB or WRGB value.
uint32_t gamma32(uint32_t x)
{
    uint8_t *y = (uint8_t *)&x;
    // All four bytes of a 32-bit value are filtered even if RGB (not WRGB),
    // to avoid a bunch of shifting and masking that would be necessary for
    // properly handling different endianisms (and each byte is a fairly
    // trivial operation, so it might not even be wasting cycles vs a check
    // and branch for the RGB case). In theory this might cause trouble *if*
    // someone's storing information in the unused most significant byte
    // of an RGB value, but this seems exceedingly rare and if it's
    // encountered in reality they can mask values going in or coming out.
    for (uint8_t i = 0; i < 4; i++)
        y[i] = gamma8(y[i]);
    return x; // Packed 32-bit return
}

/*!
  @brief   Fill NeoPixel strip with one or more cycles of hues.
           Everyone loves the rainbow swirl so much, now it's canon!
  @param   first_hue   Hue of first pixel, 0-65535, representing one full
                       cycle of the color wheel. Each subsequent pixel will
                       be offset to complete one or more cycles over the
                       length of the strip.
  @param   reps        Number of cycles of the color wheel over the length
                       of the strip. Default is 1. Negative values can be
                       used to reverse the hue order.
  @param   saturation  Saturation (optional), 0-255 = gray to pure hue,
                       default = 255.
  @param   brightness  Brightness/value (optional), 0-255 = off to max,
                       default = 255. This is distinct and in combination
                       with any configured global strip brightness.
  @param   gammify     If true (default), apply gamma correction to colors
                       for better appearance.
*/
void rainbow(uint16_t first_hue, int8_t reps,
             uint8_t saturation, uint8_t brightness, uint8_t gammify)
{
    for (uint16_t i = 0; i < numLEDs; i++)
    {
        uint16_t hue = first_hue + (i * reps * 65536) / numLEDs;
        uint32_t color = ColorHSV(hue, saturation, brightness);
        if (gammify)
            color = gamma32(color);
        setPixelColor(i, color);
    }
}

uint16_t numPixels(void) { return numLEDs; }
static uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

/*!
  @brief   Transmit pixel data in RAM to NeoPixels.
  @note    On most architectures, interrupts are temporarily disabled in
           order to achieve the correct NeoPixel signal timing. This means
           that the Arduino millis() and micros() functions, which require
           interrupts, will lose small intervals of time whenever this
           function is called (about 30 microseconds per RGB pixel, 40 for
           RGBW pixels). There's no easy fix for this, but a few
           specialized alternative or companion libraries exist that use
           very device-specific peripherals to work around it.
*/
void show(void)
{
    uint8_t *p;
    pixel_rgb_t c;

    for (uint16_t i = 0; i < numLEDs; i++)
    {
        if (wOffset == rOffset)
        {
            p = &pixels[i * 3];
        }
        else
        {
            p = &pixels[i * 4];
        }
        c.r = p[rOffset];
        c.g = p[gOffset];
        c.b = p[bOffset];
        led_matrix_set_color(i, c);
    }
    led_matrix_reflash();
}

static neo_pixel_tools_t _tools =
{
    .ColorHSV = ColorHSV,
    .gamma32 = gamma32,
    .rainbow = rainbow,
};

static neo_pixel_ops_t _ops =
{
    .setPixelColor = setPixelColor,
    .setPixelColorRGB = setPixelColorRGB,
    .setPixelColorRGBW = setPixelColorRGBW,
    .fill = fill,
    .setBrightness = setBrightness,
    .clear = clear,
    .show = show,
    .updateLength = updateLength,
    .tool = &_tools,
};

/**
 * @brief init the neo pixel and get pixel ops.
 * 
 * @param ops ops for pixel control
 * @param n  Number of RGB LEDs in strip
 */
void neo_pixel_init(neo_pixel_ops_t **ops, uint16_t n)
{
    brightness = 0;
    pixels = RT_NULL;
    rOffset = 1;
    gOffset = 0;
    bOffset = 2;
    wOffset = 1;
    updateLength(n);
    *ops = &_ops;
    // TODO init LED matrix
}

#endif /* defined(BSP_USING_LED_MATRIX) && defined(BSP_USING_NEOPIXEL) */
