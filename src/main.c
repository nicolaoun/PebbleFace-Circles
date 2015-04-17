#include <pebble.h>
  
#define HOURS_RADIUS 60
#define MIN_RADIUS 36
#define SEC_RADIUS 20
#define HOURS_THICK 6
#define MIN_THICK 4
#define SEC_THICK 2

  typedef struct {
  int hours;
  int minutes;
  int seconds;
} Time;

Window *my_window;
TextLayer *text_layer;
TextLayer *hour_text_layer, *min_text_layer, *sec_text_layer;
Layer *canvas_layer;
Layer *right_hours_layer, *left_hours_layer;
Layer *right_min, *left_min;
Time current_time;
GPoint w_center, canvas_center;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
  GRect layer_bounds = layer_get_bounds(right_hours_layer);
  
  layer_set_bounds(right_hours_layer, GRect(layer_bounds.origin.x, layer_bounds.origin.y, layer_bounds.size.w, layer_bounds.size.h/2 )); 
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

void click_config_provider(void *context) {
  text_layer_set_text(text_layer, "Setting the text");
  
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/************************************ UI **************************************/

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Store time
  current_time.hours = tick_time->tm_hour;
  current_time.minutes = tick_time->tm_min;
  current_time.seconds = tick_time->tm_sec;

  // Redraw
  if(canvas_layer) {
    layer_mark_dirty(canvas_layer);
  }
}

void draw_hour_layers(int hours, Layer *canvas, GContext *ctx){
  int norm_hours=0;
  float inc_step;
  float top_y = canvas_center.y - HOURS_RADIUS - HOURS_THICK;
  float max_height = 2 * ( HOURS_RADIUS + HOURS_THICK );
  float half_circle = HOURS_RADIUS + HOURS_THICK;
  GRect c_bounds = layer_get_bounds(canvas);
  
  //draw the circle
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorRed);
  graphics_context_set_stroke_width(ctx, HOURS_THICK);
  
  graphics_draw_circle(ctx, canvas_center, HOURS_RADIUS);
  
  //draw covering rectangles
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 0);
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  // normalize hours between 0-12
  norm_hours = (hours >= 12)? hours - 12 : hours;
  
  //calculate the size of the circle piece 
  inc_step = max_height / 6;
  
  //draw right square
  float r_height = 0, y_origin = top_y; 
  if (norm_hours < 6){ 
    r_height = max_height - inc_step * norm_hours;
    y_origin = y_origin + inc_step * norm_hours;
  }
  
  graphics_fill_rect(ctx, GRect(canvas_center.x, y_origin , half_circle, r_height),0,0); 
  
  //draw left layer
  float l_height = (norm_hours > 6 && norm_hours < 12)? max_height - (inc_step*(norm_hours-6)) : max_height;
  
  graphics_fill_rect(ctx, GRect(canvas_center.x-half_circle, top_y , half_circle, l_height),0,0);
}

void draw_min_layers(int min, Layer *canvas, GContext *ctx){
  float inc_step;
  float top_y = canvas_center.y - MIN_RADIUS - MIN_THICK/2;
  float max_height = 2 * ( MIN_RADIUS + MIN_THICK/2 );
  float half_circle = MIN_RADIUS + MIN_THICK;
  
  //draw the circle
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorBlue);
  graphics_context_set_stroke_width(ctx, MIN_THICK);
  
  graphics_draw_circle(ctx, canvas_center, MIN_RADIUS);
  
  //draw covering rectangles
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 0);
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  //calculate the size of the circle piece 
  inc_step = max_height / 30;
  
  //draw right square
  float r_height = 0, y_origin = top_y; 
  if (min < 30){ 
    r_height = max_height - inc_step * min + 1;
    y_origin = y_origin + inc_step * min;
  }
  
  graphics_fill_rect(ctx, GRect(canvas_center.x, y_origin , half_circle, r_height),0,0); 
  
  //draw left layer
  float l_height = (min > 30 && min <= 60)? max_height - (inc_step*(min-30)) : max_height;
  
  graphics_fill_rect(ctx, GRect(canvas_center.x-half_circle, top_y , half_circle, l_height),0,0);
}

void draw_sec_layers(int sec, Layer *canvas, GContext *ctx){
  float inc_step;
  float top_y = canvas_center.y - SEC_RADIUS - SEC_THICK/2;
  float max_height = 2 * ( SEC_RADIUS + SEC_THICK );
  float half_circle = SEC_RADIUS + SEC_THICK;
  
  //draw the circle
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorPurple);
  graphics_context_set_stroke_width(ctx, SEC_THICK);
  
  graphics_draw_circle(ctx, canvas_center, SEC_RADIUS);
  
  //draw covering rectangles
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 0);
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  //calculate the size of the circle piece 
  inc_step = max_height / 30;
  
  //draw right square
  float r_height = 0, y_origin = top_y; 
  if (sec < 30){ 
    r_height = max_height - inc_step * sec + 1;
    y_origin = y_origin + inc_step * sec;
  }
  
  graphics_fill_rect(ctx, GRect(canvas_center.x, y_origin , half_circle, r_height),0,0); 
  
  //draw left rectangle
  float l_height = (sec > 30 && sec <= 60)? max_height - (inc_step*(sec-30)) : max_height;
  
  graphics_fill_rect(ctx, GRect(canvas_center.x-half_circle, top_y , half_circle, l_height),0,0);
}

static void draw_text_layers(Layer *canvas, GContext *ctx) {
	
}

//canvas update procedure
void my_update_proc(Layer *layer, GContext *ctx) {  
  // draw hour circle
  draw_hour_layers(current_time.hours, layer, ctx);
  
  // draw min circle
  draw_min_layers(current_time.minutes, layer, ctx);
  
  // draw sec circle
  draw_sec_layers(current_time.seconds, layer, ctx);
  
}

void deinit_canvas_layers(){
  layer_destroy(right_hours_layer);
  layer_destroy(left_hours_layer);
}


void window_load(){
  Layer * win_layer = window_get_root_layer(my_window);
  GRect win_bounds = layer_get_bounds(win_layer);
  
  w_center = GPoint(win_bounds.size.w/2, win_bounds.size.h/2);
    
  //add canvas that will hold everything
  canvas_layer = layer_create(GRect(0,5,win_bounds.size.w,win_bounds.size.h-10));
  layer_add_child(win_layer, canvas_layer);
  layer_set_update_proc(canvas_layer, my_update_proc);
  canvas_center = GPoint( win_bounds.size.w/2, (10+win_bounds.size.h)/2);
  
  //add text layer
  text_layer = text_layer_create(GRect((win_bounds.size.w-144)/2, 0, 144, 20));
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(win_layer, text_layer_get_layer(text_layer));

  //add hour text layer
  hour_text_layer = text_layer_create(GRect(canvas_center.x-10, canvas_center.y-(HOUR_RADIUS+HOUR_THICK+15), 20, 30));
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  layer_add_child(win_layer, text_layer_get_layer(hour_text_layer));

  //add min text layer
  min_text_layer = text_layer_create(GRect(canvas_center.x-10, canvas_center.y-(MIN_RADIUS+MIN_THICK+15), 20, 30));
  text_layer_set_text_alignment(min_text_layer, GTextAlignmentCenter);
  layer_add_child(win_layer, text_layer_get_layer(min_text_layer));


  //add sec text layer
  sec_text_layer = text_layer_create(GRect(canvas_center.x-10, canvas_center.y-(SEC_RADIUS+SEC_THICK+15), 20, 30));
  text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter);
  layer_add_child(win_layer, text_layer_get_layer(sec_text_layer));



}

void window_unload(){
  text_layer_destroy(text_layer);
  layer_destroy(canvas_layer);
  //deinit_hour_layers();
}

/*********************************** App **************************************/

void handle_init(void) {
  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  tick_handler(time_now, SECOND_UNIT);
  
  my_window = window_create();

  window_set_click_config_provider(my_window, (ClickConfigProvider) click_config_provider);
  window_set_window_handlers(my_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
                               
  window_stack_push(my_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
