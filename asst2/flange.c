//
// FLANGE, the FLAN graphics engine.
//
// Based on: Peter Henderson, "Functional Geometry", Symposium on LISP
// and Functional Programming, 1982.
// http://doi.acm.org/10.1145/800068.802148
//
// A more recent account: Peter Henderson, "Functional Geometry",
// Higher-Order and Symbolic Computation 15(4): 349-365 (2002).
// http://dx.doi.org/10.1023/A:1022986521797
//
// Also: P. R. Eggert and K. P. Chow, "Logic programming graphics and
// infinite terms", Technical Report, Department of Computer Science,
// University of California, Santa Barbara, June 1983.
//
// Henderson's work implemented in Lisp, from which I scraped the
// concrete pictures:
//
//    http://www.frank-buss.de/lisp/functional.html
//
// There is also some information in Abelson and Sussman's "Structure
// and Interpretation of Computer Programs".
//
// Peter Gammie, peteg@cse.unsw.edu.au
// Commenced August 2008.
//
//

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "environment.h"
#include "errors.h"
#include "eval.h"
#include "flange.h"
#include "graphics.h"
#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "types.h"
#include "values.h"

//**************************************
// Renderer state.

typedef struct {
  graphics_t *g;
  // Henderson's three vectors.
  vector_t origin; // a: Lower-left corner of the box.
  vector_t bottom; // b: Bottom of the picture.
  vector_t left;   // c: Left edge of the picture.
} render_t;


//**************************************
// Top-level: render a FLANGE picture.

static list_t *flange_types(void);
static env_t *flange_bindings(void);
static void f_render(render_t *r, value_t *v);

void flange_render(int type_check, graphics_t *g, program_t *program,
                   value_t *(*evaluate_program)(env_t *env, program_t *program))
{
  double width = 600, height = 600;
  list_t *fts = flange_types();
  env_t *env = flange_bindings();
  render_t r;

  // Append the FLANGE-specific types.
  list_append(&program->types, &fts);

  if (type_check && !types_check(program)) {
    fprintf(stdout, " ** Aborting on type error.\n");
    exit(1);
  }

  // Bottom-left coordinate is (0,0); top-right is (width,height).
  r.g = g;
  r.origin.x = 0;
  r.origin.y = 0;

  r.bottom.x = width;
  r.bottom.y = 0;

  r.left.x = 0;
  r.left.y = height;

  // Draw a picture.
  graphics_picture_start(g, width, height);
#if 1
  // TODO
  // when your rendering code is working,
  // replace the above line by #if 1
  f_render(&r, evaluate_program(env, program));
#else
  print_value(stdout,evaluate_program(env, program));
  fprintf(stdout,"\n");
#endif
  graphics_picture_end(g);
}

//**************************************
// Vector arithmetic over vector_t.

static vector_t vec_add(vector_t a, vector_t b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

static vector_t vec_sub(vector_t a, vector_t b) {
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

static vector_t vec_scale(vector_t a, double c) {
  a.x *= c;
  a.y *= c;
  return a;
}

static double vec_length(vector_t a) {
  return hypot(a.x, a.y);
}

static double vec_abs_cross_product(vector_t a, vector_t b) {
  return fabs(a.x * b.y - a.y * b.x);
}

//**************************************
// FLANGE drawing and geometry types and functions.
//
// Extend the standard builtin functions and types with
// FLANGE-specific ones. Use the parser so we don't have to build the
// data structures tediously by hand.
//
static char *flangeTypes =
  "data Shape = Circle (Num, Num) Num            \
              | Line (Num, Num) (Num, Num)       \
              | Rectangle (Num, Num) (Num, Num)  \
              | FilledPolygon (Num, Num, Num)    \
                              [(Num, Num)]       \
   data Picture                                  \
              = Above Num Num Picture Picture    \
              | Beside Num Num Picture Picture   \
              | Canvas Num Num [Shape]           \
              | Empty                            \
              | Flip Picture                     \
              | Overlay Picture Picture          \
              | Rotate Picture";

static list_t *flange_types(void)
{
  list_t *types;

  lex_string_start(flangeTypes);
  lex();
  types = typedefs();
  lex_string_end();

  return types;
}

static env_t *flange_bindings(void)
{
  return builtin_bindings();
}

//**************************************
// Render Canvas shapes.

typedef struct {
  render_t *r;
  double width, height;
} canvas_closure_t;

static void render_circle(canvas_closure_t *c, vector_t centre, double radius)
{
  render_t *r = c->r;
  // transformed origin and radius using render vectors
  vector_t newOrigin;
  double newxRadius, newyRadius;

  newOrigin = vec_add(
                  vec_add(r->origin, vec_scale(r->bottom, centre.x / c->width)),
                  vec_scale(r->left, centre.y / c->height));
  newxRadius = vec_length(vec_scale(r->bottom, radius / c->width));
  newyRadius = vec_length(vec_scale(r->left,   radius / c->height));

  graphics_ellipse(r->g, newOrigin, newxRadius, newyRadius, r->bottom);
}

static void render_line(canvas_closure_t *c, vector_t j, vector_t k)
{
  render_t *r = c->r;
  // transformed origin and radius using render vectors
  vector_t newStart, newEnd;

  newStart = vec_add(
                  vec_add(r->origin, vec_scale(r->bottom, j.x / c->width)),
                  vec_scale(r->left, j.y / c->height));
  newEnd = vec_add(vec_add(r->origin, vec_scale(r->bottom, k.x / c->width)),
                	vec_scale(r->left, k.y / c->height));

  graphics_line(r->g, newStart, newEnd);
}

static void render_poly_emit_point(canvas_closure_t *c, vector_t v) {
  render_t *r = c->r;
  // transformed origin and radius using render vectors
  vector_t coord = vec_add(
    vec_add(r->origin, vec_scale(r->bottom, v.x / c->width)),
    vec_scale(r->left, v.y / c->height)
  );

  graphics_poly_emit_point(c->r->g, coord);
}

static int render_shape(canvas_closure_t *c, value_t *shape)
{
  switch (shape->type) {
  default:
  case v_unused:
    if (*(int *)NULL) { printf("should crash.\n"); }
    break;
  case v_datacons:
    if (strcmp(datacons_tag(shape), "Circle") == 0) {
      double radius;
      vector_t center;

      // Grab the datacons
      value_t *val_center = list_nth(datacons_params(shape), 0);
      value_t *val_rad = list_nth(datacons_params(shape), 1);

      // Extract the data
      center.x = num_val((value_t*)list_nth(tuple_val(val_center), 0));
      center.y = num_val((value_t*)list_nth(tuple_val(val_center), 1));
      radius = num_val(val_rad);

      // And render
      render_circle(c, center, radius);
    }
    else if (strcmp(datacons_tag(shape), "Line") == 0) {
      vector_t start;
      vector_t end;

      // Grab the datacons
      value_t *val_first = list_nth(datacons_params(shape), 0);
      value_t *val_second = list_nth(datacons_params(shape), 1);
      list_t *tup_first = tuple_val(val_first);
      list_t *tup_second = tuple_val(val_second);

      // Extract the data
      start.x = num_val((value_t*)list_nth(tup_first, 0));
      start.y = num_val((value_t*)list_nth(tup_first, 1));
      end.x = num_val((value_t*)list_nth(tup_second, 0));
      end.y = num_val((value_t*)list_nth(tup_second, 1));

      // And render
      render_line(c, start, end);
    } else if (strcmp(datacons_tag(shape), "Rectangle") == 0) {
      vector_t start, end, cornerA, cornerB;

      // Grab the datacons
      value_t *val_first = list_nth(datacons_params(shape), 0);
      value_t *val_second = list_nth(datacons_params(shape), 1);
      list_t *tup_first = tuple_val(val_first);
      list_t *tup_second = tuple_val(val_second);

      // Extract the data
      start.x = num_val((value_t*)list_nth(tup_first, 0));
      start.y = num_val((value_t*)list_nth(tup_first, 1));
      end.x = num_val((value_t*)list_nth(tup_second, 0));
      end.y = num_val((value_t*)list_nth(tup_second, 1));

      cornerA.x = start.x;
      cornerA.y = end.y;
      cornerB.x = end.x;
      cornerB.y = start.y;

      // And render as 4 lines.
      render_line(c, start, cornerA);
      render_line(c, cornerA, end);
      render_line(c, end, cornerB);
      render_line(c, cornerB, start);
    } else if (strcmp(datacons_tag(shape), "FilledPolygon") == 0) {
      // Grab the datacons
      value_t *val_colour = list_nth(datacons_params(shape), 0);
      list_t *tup_colour = tuple_val(val_colour);

      // Colour...
      double red, gre, blu;
      red = num_val((value_t*)list_nth(tup_colour, 0));
      gre = num_val((value_t*)list_nth(tup_colour, 1));
      blu = num_val((value_t*)list_nth(tup_colour, 2));

      graphics_poly_begin(c->r->g, red, gre, blu);

      // WTFlan. Dark magic inside this loop.
      value_t *v = list_nth((list_t*)datacons_params(shape), 1);
      while (v->type == v_datacons
            && (strcmp(datacons_tag(v), listConsTag) == 0)) {
        value_t *temp = list_head(datacons_params(v));
        list_t *tup = tuple_val(temp);

        vector_t point;
        point.x = num_val((value_t*)list_nth(tup, 0));
        point.y = num_val((value_t*)list_nth(tup, 1));
        render_poly_emit_point(c, point);

        v = thunk_force(list_nth(datacons_params(v), 1));
      }

      graphics_poly_end(c->r->g);
    } else {
      printf("render_shape: unknown shape.\n");
      print_value(stdout, shape);
      error("");
    }
    break;
  }
  return 1;
}

//**************************************
// The recursive renderer.

static void f_render(render_t *r, value_t *v);

static void f_empty(render_t *r, value_t *v)
{
  // does nothing (intentionally left blank)
}

static void f_canvas(render_t *r, value_t *v)
{
  // Canvas width height shapes
  // place all shapes from list into width x height window

  list_t *params = datacons_params(v);
  double width  = num_val((value_t *)list_nth(params, 0));
  double height = num_val((value_t *)list_nth(params, 1));
  value_t *shapes = (value_t *)list_nth(params, 2);
  canvas_closure_t c;

  c.r = r;
  c.width = width;
  c.height = height;

  while (shapes->type == v_datacons
        && strcmp(datacons_tag(shapes), listConsTag) == 0) {
    render_shape(&c, list_nth(datacons_params(shapes), 0));
    shapes = (value_t *)list_nth(datacons_params(shapes), 1);
  }
}

static void f_above(render_t *r, value_t *v)
{
  // Above n m p q
  // places Picture p above Picture q
  // p occupies n/(n+m) of window height
  // q occupies m/(n+m) of window height

  double n = num_val((value_t*)list_nth(datacons_params(v), 0));
  double m = num_val((value_t*)list_nth(datacons_params(v), 1));

  render_t top = *r, bottom = *r;

  bottom.left = vec_scale(bottom.left, m/(n+m));
  top.left    = vec_scale(top.left, n/(n+m));
  top.origin  = vec_add(top.origin, bottom.left);

  f_render(&top,    list_nth(datacons_params(v), 2));
  f_render(&bottom, list_nth(datacons_params(v), 3));
}

static void f_beside(render_t *r, value_t *v)
{
  // Beside n m p q
  // places Picture p beside Picture q
  // p occupies n/(n+m) of window width
  // q occupies m/(n+m) of window width

  double n = num_val((value_t*)list_nth(datacons_params(v), 0));
  double m = num_val((value_t*)list_nth(datacons_params(v), 1));

  render_t left = *r, right = *r;

  left.bottom  = vec_scale(left.bottom, n/(n+m));
  right.bottom = vec_scale(right.bottom, m/(n+m));
  right.origin = vec_add(right.origin, left.bottom);

  f_render(&left,  list_nth(datacons_params(v), 2));
  f_render(&right, list_nth(datacons_params(v), 3));
}

static void f_flip(render_t *r, value_t *v)
{
  // Flip p
  // reflect p in vertical axis bisecting window

  value_t *param = list_nth(datacons_params(v), 0);

  render_t flipped = *r;
  flipped.origin = vec_add(flipped.origin, flipped.bottom);
  flipped.bottom = vec_scale(flipped.bottom, -1);

  f_render(&flipped, param);
}

static void f_overlay(render_t *r, value_t *v)
{
  // Overlay p q
  // picture containing all elements of p and q

  f_render(r, list_nth(datacons_params(v), 0));
  f_render(r, list_nth(datacons_params(v), 1));
}

static void f_rotate(render_t *r, value_t *v)
{
  // Rotate p
  // rotate p anticlockwise by 90 degree

  value_t *param = list_nth(datacons_params(v), 0);

  double len_ratio = vec_length(r->left) / vec_length(r->bottom);

  render_t rotated;
  rotated.g = r->g;

  rotated.left.x    = -1 * r->left.y;
  rotated.left.y    =      r->left.x;
  rotated.left      = vec_scale(rotated.left, 1/len_ratio);

  rotated.bottom.x  = -1 * r->bottom.y;
  rotated.bottom.y  =      r->bottom.x;
  rotated.bottom    = vec_scale(rotated.bottom, len_ratio);

  rotated.origin    = vec_sub(r->origin, rotated.left);

  f_render(&rotated, param);
}

static void f_render(render_t *r, value_t *v)
{
  switch (v->type) {
  default:
  case v_unused:
    error("flange: Expected 'main' to be bound to an expression of type 'Picture'.\n");
    break;
  case v_datacons:
    if (strcmp(datacons_tag(v), "Above") == 0) {
      f_above(r, v);
    } else if (strcmp(datacons_tag(v), "Beside") == 0) {
      f_beside(r, v);
    } else if (strcmp(datacons_tag(v), "Canvas") == 0) {
      f_canvas(r, v);
    } else if (strcmp(datacons_tag(v), "Empty") == 0) {
      f_empty(r, v);
    } else if (strcmp(datacons_tag(v), "Flip") == 0) {
      f_flip(r, v);
    } else if (strcmp(datacons_tag(v), "Overlay") == 0) {
      f_overlay(r, v);
    } else if (strcmp(datacons_tag(v), "Rotate") == 0) {
      f_rotate(r, v);
    } else {
      error("flange: Unknown data constructor: '%s'.\n", datacons_tag(v));
    }
    break;
  }
}
