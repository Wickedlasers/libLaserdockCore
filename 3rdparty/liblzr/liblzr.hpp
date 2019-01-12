

/******************************************************************************/
/*                                                                            */
/*  liblzr - a suite of common tools for handling laser graphics              */
/*                                                                            */
/******************************************************************************/



#pragma once

#pragma warning(push, 0)

#define LZR_VERSION "0.0.1"

// Windows dll export symbols
#ifdef _WIN32
#define LIBLZR_EXPORT __declspec(dllexport)
#else
#define LIBLZR_EXPORT
#endif


#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include <string>

namespace lzr {



/******************************************************************************/
/*  LZR Return Codes                                                          */
/******************************************************************************/

#define LZR_SUCCESS            0
#define LZR_WARNING           -1
#define LZR_FAILURE           -2
#define LZR_ERROR_INVALID_ARG -3



/******************************************************************************/
/*  LZR Points                                                                */
/******************************************************************************/

//point limits
#define LZR_POSITION_MIN -1.0
#define LZR_POSITION_MAX 1.0
#define LZR_COLOR_MIN    0
#define LZR_COLOR_MAX    255

/*
 * Coordinate system:
 *
 *  (-1, 1)        (1, 1)
 *      _____________
 *     |      |      |
 *     |      |      |
 *     |      |      |
 *     |------+------|
 *     |      |      |
 *     |      |      |
 *     |______|______|
 *
 * (-1, -1)        (1, -1)
 *
 */

class LIBLZR_EXPORT Point
{
public:
    double x;  //Position X   [-1.0, 1.0]
    double y;  //Position Y   [-1.0, 1.0]
    uint8_t r; //Red          [0, 255]
    uint8_t g; //Green        [0, 255]
    uint8_t b; //Blue         [0, 255]
    uint8_t i; //Blanking     [0, 255]

    Point();
    Point(double x, double y);
    Point(double x, double y, uint8_t r, uint8_t g, uint8_t b, uint8_t i);

    void blank();
    void unblank();
    void set_position(const Point& other);
    void set_color(const Point& other);
    bool is_blanked() const;
    bool is_lit() const;
    Point lerp_to(const Point& other, double t) const;
    double sq_distance_to(const Point& other) const;
    bool same_position_as(const Point& other) const;
    bool same_color_as(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
};



/******************************************************************************/
/*  LZR Frames                                                                */
/******************************************************************************/

//OK as long as the Frame class doesn't save any state, which, it shouldn't...
//this is only a convenient way to attach functions to the type
class LIBLZR_EXPORT Frame : public std::vector<Point>
{
public:
    Frame();
    Frame(size_t n);

    Frame& add(const Point& p);
    Frame& add(const Frame& other);
    Frame& add_with_blank_jump(const Point& p);
    Frame& add_with_blank_jump(const Frame& other);

    void bounding_box(Point& min, Point& max) const;
    Point bounding_box_center() const;
    Point average_center() const;

private:
    Frame& add_blank_jump_to(const Point& p);
    Frame& add_blank_jump_to(const Frame& other);
};


/******************************************************************************/
/*  LZR Animations                                                            */
/******************************************************************************/

typedef std::vector<Frame> FrameList;



/******************************************************************************/
/*  LZR Frame Transforms                                                      */
/******************************************************************************/

LIBLZR_EXPORT int translate(Frame& frame, double x, double y);
LIBLZR_EXPORT int rotate(Frame& frame, Point center, double theta);
LIBLZR_EXPORT int scale(Frame& frame, Point center, double x, double y);
LIBLZR_EXPORT int mirror(Frame& frame, Point center, bool x, bool y);
LIBLZR_EXPORT int dup_mirror(Frame& frame, Point center, bool x, bool y, bool blank=true);
LIBLZR_EXPORT int dup_linear(Frame& frame, Point offset, size_t n_dups, bool blank=true);
LIBLZR_EXPORT int dup_radial(Frame& frame, Point center, size_t n_dups, double angle, bool blank=true);


//clips a frame using the given mask. Points in the mask should define a closed
//polygon. All points outside the mask are discarded. Line segments that cross
//the mask boundry will have additional points inserted at that boundry.
LIBLZR_EXPORT int mask(Frame& frame, Frame mask, bool inverse=false);



/******************************************************************************/
/*  LZR Optimizer                                                             */
/*  Converts a stream of raw frames into something friendly for the scanners  */
/*  Handles:                                                                  */
/*    - interpolation (for both blanked and lit lines)                        */
/*    - anchor points                                                         */
/*    - path scan order                                                       */
/*    - inter-frame blanking                                                  */
/******************************************************************************/

//interpolation point density (points from one side of the frame to the other)
#define INTERP_DEFAULT ((LZR_POSITION_MAX - LZR_POSITION_MIN) / 100.0)
#define BLANK_INTERP_DEFAULT ((LZR_POSITION_MAX - LZR_POSITION_MIN) / 5.0)

//interpolation functions
typedef double (*interpolation_func)(double t);
LIBLZR_EXPORT double linear(double t); /*----*----*----*----*----*----*----*----*/
LIBLZR_EXPORT double quad(double t);   /*---*---*-----*-----*-----*-----*---*---*/
LIBLZR_EXPORT double quart(double t);  /*-*---*-----*-------*-------*-----*---*-*/

//fwrd decl
class OptimizerInternals;

class LIBLZR_EXPORT Optimizer
{
public:
    Optimizer();
    Optimizer(Point current_laser_position);
    ~Optimizer();

    //main optimizer function
    int run(Frame& frame);

    // ----- settings -----

    double path_split_angle = 45; //minimum angle (degrees) at which to consider lines to be seperate paths
    bool   reorder_paths = true;  //allow the optimizer to the change the order in which points are scanned

    //anchor points
    size_t anchor_points_lit = 1;      //minimum number of lit points to place at the start & end of line segments
    size_t anchor_points_blanked  = 2; //minimum number of blanked points to place at the start & end of a line segment

    //interpolation
    double interp_distance = INTERP_DEFAULT; //max distance for interpolation of lit lines
    interpolation_func interp_func = linear; //interpolation function to use for lit lines

    //blanking interpolation
    double blank_interp_distance = BLANK_INTERP_DEFAULT; //max distance for interpolation of blanking jumps
    interpolation_func blank_interp_func = linear;       //interpolation function to use for blanking jumps

private:
    OptimizerInternals* internal;
};



/******************************************************************************/
/*  LZR Reducers (inverse functions for the optimization above)               */
/******************************************************************************/

//deletes unwanted points from the given frame

//removes duplicate lit points (anchor points and beams)
LIBLZR_EXPORT int reduce_duplicates(Frame& frame);

//removes interpolation from straight, lit lines
LIBLZR_EXPORT int reduce_interpolation(Frame& frame);

//removes all intersticial blanked points, regardless of path
LIBLZR_EXPORT int reduce_blanks(Frame& frame);



/******************************************************************************/
/*  ILDA File Handlers                                                        */
/******************************************************************************/

class ILDA;

// "r" = read
// "w" = write
//Will return NULL on failure
LIBLZR_EXPORT ILDA* ilda_open(const char* filename, const char* mode);

//closes the ILDA file, and releases the parsing context
LIBLZR_EXPORT int ilda_close(ILDA* f);

//Reads all frames for the the given projector, and returns them
//The array you pass to "name" or "company" must be at least 9 chars in length
LIBLZR_EXPORT int ilda_read(ILDA* f, size_t pd, FrameList& frame_list);
LIBLZR_EXPORT int ilda_read(ILDA* f, size_t pd, FrameList& frame_list, char* name, char* company);

//write frame(s) for the given projector to the ILDA file (file must be opened with lzr_ilda_write() )
//if no name/company strings are given, the 
LIBLZR_EXPORT int ilda_write(ILDA* f, size_t pd, Frame& frame);
LIBLZR_EXPORT int ilda_write(ILDA* f, size_t pd, Frame& frame, const char* name, const char* company);
LIBLZR_EXPORT int ilda_write(ILDA* f, size_t pd, FrameList& frame_list);
LIBLZR_EXPORT int ilda_write(ILDA* f, size_t pd, FrameList& frame_list, const char* name, const char* company);

//returns the number of projectors that the ILDA specifies graphics for
LIBLZR_EXPORT size_t ilda_projector_count(ILDA* f);

//returns the number of frames for the given projector descriptor
LIBLZR_EXPORT size_t ilda_frame_count(ILDA* f, size_t pd);

//return the error string, set whenever the functions above return an error
LIBLZR_EXPORT const char* ilda_error(ILDA* ilda);



/******************************************************************************/
/*  LZR DAC Handling                                                          */
/******************************************************************************/

//fwrd decl
class DAC_Internals;

//the standard interface for DACS
//do NOT instantiate this class yourself
//use the dac_connect() factory function below
class DAC
{
public:
    DAC(std::string name);
    virtual ~DAC();

    //standard DAC interface
    std::string name();
    bool connected();

    //DAC specific functions
    virtual int send(Frame frame) = 0;
    virtual int stop() = 0;
    virtual void pps(int pps);
    virtual int pps();

protected:
    DAC_Internals* internal;
};


//initialize individual DAC backends / prepare for hardware probing
void init_dacs();

//get list of connected DACs
std::vector<std::string> list_dacs();

//connect to a DAC
DAC* dac_connect(std::string name);

} // namespace lzr

#pragma warning(pop)
