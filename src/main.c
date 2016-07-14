#include <pebble.h>

Window *main_window;

//Drawing Canvas
static Layer *s_canvas_layer; 

#define MARGIN 30
#define MARGIN_SMALL 5
#define HOUR_HEIGHT 30
#define MINUTE_HEIGHT 15

int rec_count = MARGIN;

/***********************************************
* Draw a rectangle
***********************************************/
static void draw_rectangle(GContext *ctx, GRect bounds, int height, int increment, int value, int offset, bool start_left, GColor8 color) {
  // Set the line color
  graphics_context_set_stroke_color(ctx, color);

  // Set the fill color
  graphics_context_set_fill_color(ctx, color);
 
  // Draw Outline
  graphics_draw_rect(ctx, GRect(bounds.origin.x, bounds.origin.y + offset, (int)(((float) value / (float) increment) * (float) bounds.size.w), height));
  
  // Draw fill
  graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y + offset, (int)(((float) value / (float) increment) * (float) bounds.size.w), height), 0, GCornersAll);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "width is %d and length is %d", bounds.size.w, (int)(((float) value / (float) increment) * (float) bounds.size.w));
  
  // Draw the line for label
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 3);
  GPoint start = GPoint(bounds.origin.x + (int)(((float) value / (float) increment) * (float) bounds.size.w), bounds.origin.y + offset + height);
  GPoint end = GPoint(bounds.origin.x + (int)(((float) value / (float) increment) * (float) bounds.size.w), bounds.origin.y + offset - 1);
  graphics_draw_line(ctx, start, end);
  
  // Reset the stroke
  graphics_context_set_stroke_width(ctx, 1);
  
  // Redraw
  layer_mark_dirty(s_canvas_layer);
}

/***********************************************
* Update the canvas layer
***********************************************/
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Get bounds
  GRect bounds = layer_get_bounds(layer);
  
  // Set the line color
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  // Get sceen center
  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  
  // Draw Hour
  draw_rectangle(ctx,bounds,HOUR_HEIGHT, 24, tick_time->tm_hour, MARGIN, true, GColorWhite);
  
  // Draw the minute
  draw_rectangle(ctx,bounds,MINUTE_HEIGHT, 60, tick_time->tm_min, MARGIN * 2 + MARGIN_SMALL, true, GColorLightGray);
  
//   graphics_context_set_stroke_width(ctx, 5);
//   graphics_draw_circle(ctx, center, 50);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  
  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    
  // Set background color
  window_set_background_color(main_window, GColorBlack);
  
  //Add the canvas layer
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  
  // Redraw this as soon as possible
  layer_mark_dirty(s_canvas_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

void handle_init(void) {
  main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(main_window, true);
}

void handle_deinit(void) {
  window_destroy(main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
