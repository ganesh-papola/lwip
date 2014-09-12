#ifndef LWIP_IMAGE_H
#define LWIP_IMAGE_H

#define cimg_display 0
#define cimg_verbosity 0

#include <string>
#include <cmath>
#include <node.h>
#include <node_buffer.h>
#include <nan.h>
#include <v8.h>
#include "CImg.h"

using namespace cimg_library;
using namespace v8;
using namespace node;

/*
 * LwipImage object is a wrapper for a CImg object.
 * It is constructed from a memory biffer of RGB pixels.
 * The memory buffer is of type 'unsigned char' such that for an image of size
 * W * H the buffer has (W*H) R(ed) values, then (W*H) G(reen) values and then
 * (W*H) B(lue) values.
 */
class LwipImage : public node::ObjectWrap {
public:
    static void Init(Handle<Object> exports);
    static Handle<Value> NewInstance();
    static NAN_METHOD(New);
    static NAN_METHOD(resize);
    static NAN_METHOD(rotate);
    static NAN_METHOD(blur);
    static NAN_METHOD(crop);
    static NAN_METHOD(mirror);
    static NAN_METHOD(pad);
    static NAN_METHOD(width);
    static NAN_METHOD(height);
    static NAN_METHOD(buffer);
    LwipImage(unsigned char * data, size_t width, size_t height);
    ~LwipImage();
    CImg<unsigned char> * _cimg;
private:
    static Persistent<FunctionTemplate> constructor;
};

class ResizeWorker : public NanAsyncWorker {
public:
    ResizeWorker(
        size_t width,
        size_t height,
        int inter,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~ResizeWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    size_t _width;
    size_t _height;
    int _inter;
    CImg<unsigned char> * _cimg;
};

class RotateWorker : public NanAsyncWorker {
public:
    RotateWorker(
        float degs,
        int r,
        int g,
        int b,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~RotateWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    float _degs;
    int _r;
    int _g;
    int _b;
    CImg<unsigned char> * _cimg;
};

class BlurWorker : public NanAsyncWorker {
public:
    BlurWorker(
        float sigma,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~BlurWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    float _sigma;
    CImg<unsigned char> * _cimg;
};

class CropWorker : public NanAsyncWorker {
public:
    CropWorker(
        size_t left,
        size_t top,
        size_t right,
        size_t bottom,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~CropWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    size_t _left;
    size_t _top;
    size_t _right;
    size_t _bottom;
    CImg<unsigned char> * _cimg;
};

void mirrorAsync(uv_work_t * request);
void mirrorAsyncDone(uv_work_t * request, int status);
void padAsync(uv_work_t * request);
void padAsyncDone(uv_work_t * request, int status);

struct mirrorBaton {
    uv_work_t request;
    v8::Persistent<Function> cb;
    LwipImage * img;
    bool xaxis;
    bool yaxis;
    bool err;
    std::string errMsg;
};

struct padBaton {
    uv_work_t request;
    v8::Persistent<Function> cb;
    LwipImage * img;
    unsigned int left;
    unsigned int top;
    unsigned int right;
    unsigned int bottom;
    unsigned char color[3];
    bool err;
    std::string errMsg;
};

#endif
