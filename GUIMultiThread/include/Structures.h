/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* Structures.h                                                         */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/*                                                                      */
/* Copyright (c) 2012-2015 Nick D'Ademo                                 */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QRect>

typedef struct
{
    int smoothType;
    int smoothParam1;
    int smoothParam2;
    double smoothParam3;
    double smoothParam4;
    int dilateNumberOfIterations;
    int erodeNumberOfIterations;
    int flipCode;
    double cannyThreshold1;
    double cannyThreshold2;
    int cannyApertureSize;
    bool cannyL2gradient;
} ImageProcessingSettings;

typedef struct
{
    bool grayscaleOn;
    bool smoothOn;
    bool dilateOn;
    bool erodeOn;
    bool flipOn;
    bool cannyOn;
} ImageProcessingFlags;

typedef struct
{
    QRect selectionBox;
    bool leftButtonRelease;
    bool rightButtonRelease;
    int row;
    int col;
} MouseData;

typedef struct
{
    int averageFPS;
    int nFramesProcessed;
} ThreadStatisticsData;

#endif // STRUCTURES_H
