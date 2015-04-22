#include <pebble.h>
#include <math.h>
#include "theme.h"
  
#define SEC_RADIUS MAX_RADIUS //20
// compute minute radius so that embedding square does not overlap with sec circle
#define MIN_RADIUS (int) ((SEC_RADIUS - SEC_THICK) / sqrt(2)) - MIN_THICK //41 //34
// compute hours radius so that embedding square does not overlap with min circle
#define HOURS_RADIUS (int) ((MIN_RADIUS - MIN_THICK) / sqrt(2)) - HOURS_THICK //23 //56
  
#define COLORS true

typedef struct {
  int hours;
  int minutes;
  int seconds;
  char date[25];
} Time;

Window *my_window;
TextLayer *battery_text_layer;
InverterLayer *inv_layer;
TextLayer *hour_text_layer, *min_text_layer, *sec_text_layer, *date_text_layer;
Layer *canvas_layer;
Layer *right_hours_layer, *left_hours_layer;
Layer *right_min, *left_min;
BitmapLayer *battery_img_layer;
BitmapLayer *bluetooth_img_layer;
Time current_time;
GPoint w_center, canvas_center;

/************************************ UI **************************************/
static void bluetooth_handler(bool connected){
  GBitmap *bt_img;
  
  if (connected) {
    bt_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ON);
  }
  else {
    bt_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_OFF);
  }
  
   bitmap_layer_set_bitmap(bluetooth_img_layer, bt_img);
}

static void battery_handler(BatteryChargeState charge_state){
  static char battery_text[25];
  GBitmap *battery_img;

  GColor color;

  snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  
  if (charge_state.is_charging) {
    color = (COLORS)? GColorDarkGreen : GColorBlack;
    
    //add battery img
    battery_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_CHARGING);
  } 
  else {
    if ( charge_state.charge_percent > 50 ) {
      color = (COLORS) ? GColorDarkGreen : GColorBlack;
      if ( charge_state.charge_percent > 75 ) {
        //add battery img
        battery_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_FULL);
      }
      else {
        //add battery img
        battery_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_75);
      }
    }
    else {
      if ( charge_state.charge_percent >= 25 ) {
        color = (COLORS) ? GColorOrange : GColorBlack; 
        //add battery img
        battery_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_50);
      }
      else {
        color = (COLORS) ? GColorRed : GColorBlack;
      
        if (charge_state.charge_percent > 0)
          //add battery img
          battery_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_25);
        else
          //add battery img
          battery_img = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_EMPTY);
      }
    }
  }
  
  text_layer_set_text_color(battery_text_layer, color);
  text_layer_set_text(battery_text_layer, battery_text);
  bitmap_layer_set_bitmap(battery_img_layer, battery_img);
}

//second tick handler
static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  // Store time
  current_time.hours = tick_time->tm_hour;
  current_time.minutes = tick_time->tm_min;
  current_time.seconds = tick_time->tm_sec;
  strftime(current_time.date, sizeof(current_time.date), "%B %d", tick_time);

  // Redraw
  if(canvas_layer) {
    layer_mark_dirty(canvas_layer);
  }
}

void draw_hour_layers(int hours, Layer *canvas, GContext *ctx){
  int norm_hours=0;
  float inc_step;
  float max_height = 2 * ( HOURS_RADIUS + HOURS_THICK );
  float half_circle = HOURS_RADIUS + HOURS_THICK;
  float top_y = canvas_center.y - half_circle;
  GColor color;
  
  //draw the circle
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  color = (COLORS)? HOURS_COLOR : GColorBlack;
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, HOURS_THICK);
  
  graphics_draw_circle(ctx, canvas_center, HOURS_RADIUS);
  
  //draw covering rectangles
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  color = (COLORS)? BACKGROUND_COLOR : GColorWhite;
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 0);
  graphics_context_set_fill_color(ctx, color);
  
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
  //float top_y = canvas_center.y - MIN_RADIUS;
  float max_height = 2 * ( MIN_RADIUS + MIN_THICK );
  float half_circle = MIN_RADIUS + MIN_THICK;
  float top_y = canvas_center.y - half_circle;
  GColor color;
  
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "Min: C_y = %d, M_R= %d, Top_y = %d, Max_Height = %d, Half_Circle = %d \n", (int) canvas_center.y, (int) MIN_RADIUS, (int) top_y, (int) max_height, (int) half_circle);
  
  //draw the circle
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  color = (COLORS)? MIN_COLOR : GColorBlack;
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, MIN_THICK);
  
  graphics_draw_circle(ctx, canvas_center, MIN_RADIUS);
  
  //draw covering rectangles
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  color = (COLORS)? BACKGROUND_COLOR : GColorWhite;
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 0);
  graphics_context_set_fill_color(ctx, color);
  
  //calculate the size of the circle piece 
  inc_step = max_height / 30;
  
  //draw right square
  float r_height = 0, y_origin = top_y; 
  if (min < 30){ 
    y_origin = y_origin + inc_step * min;
    r_height = max_height - inc_step * min;
  }
  
  graphics_fill_rect(ctx, GRect(canvas_center.x, y_origin , half_circle, r_height),0,0); 
  
  //draw left layer
  float l_height = (min > 30 && min <= 60)? max_height - (inc_step*(min-30)) : max_height;
  
  graphics_fill_rect(ctx, GRect(canvas_center.x-half_circle, top_y , half_circle, l_height),0,0);
}

void draw_sec_layers(int sec, Layer *canvas, GContext *ctx){
  float inc_step;
  float max_height = 2 * ( SEC_RADIUS + SEC_THICK );
  float half_circle = SEC_RADIUS + SEC_THICK;
  float top_y = canvas_center.y - half_circle;
  GColor color;
  
  //draw the circle
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  color = (COLORS)? SEC_COLOR : GColorBlack;
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, SEC_THICK);
  
  graphics_draw_circle(ctx, canvas_center, SEC_RADIUS);
  
  //draw covering rectangles
  //set parameters
  graphics_context_set_antialiased(ctx, true);
  color = (COLORS)? BACKGROUND_COLOR : GColorWhite;
  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 0);
  graphics_context_set_fill_color(ctx, color);
  
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


//canvas update procedure
void my_update_proc(Layer *layer, GContext *ctx) {  

  static char hour[]="00";
  static char min[]="00";
  static char sec[]="00";

  // draw sec circle
  draw_sec_layers(current_time.seconds, layer, ctx);
  
  // draw min circle
  draw_min_layers(current_time.minutes, layer, ctx);
  
  // draw hour circle
  draw_hour_layers(current_time.hours, layer, ctx);
  
  // write the date
  GRect date_bounds = layer_get_bounds((Layer *) date_text_layer);
  text_layer_set_text(date_text_layer, current_time.date);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(0, 20), GPoint(date_bounds.size.w, 20));

  //draw indication lines
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  // 15 min
  graphics_draw_line(ctx, GPoint(canvas_center.x + 10, canvas_center.y), GPoint(canvas_center.x+HOURS_RADIUS+HOURS_THICK, canvas_center.y));
  // 30 min
  graphics_draw_line(ctx, GPoint(canvas_center.x, canvas_center.y+10), GPoint(canvas_center.x, canvas_center.y+HOURS_RADIUS+HOURS_THICK));
  // 45 min
  graphics_draw_line(ctx, GPoint(canvas_center.x - 10, canvas_center.y), GPoint(canvas_center.x-(HOURS_RADIUS+HOURS_THICK), canvas_center.y));
  // 60 min
  graphics_draw_line(ctx, GPoint(canvas_center.x, canvas_center.y-10), GPoint(canvas_center.x, canvas_center.y-(HOURS_RADIUS+HOURS_THICK)));

  //Write the current time on the time layers
  (current_time.hours < 10) ? snprintf(hour, sizeof(hour), "0%d", current_time.hours) : snprintf(hour, sizeof(hour), "%d", current_time.hours);
  text_layer_set_text(hour_text_layer, hour);  

  (current_time.minutes < 10) ? snprintf(min, sizeof(min), "0%d", current_time.minutes) : snprintf(min, sizeof(min), "%d", current_time.minutes);
  text_layer_set_text(min_text_layer, min);

  (current_time.seconds < 10) ? snprintf(sec, sizeof(sec), "0%d", current_time.seconds) : snprintf(sec, sizeof(sec), "%d", current_time.seconds);;
  text_layer_set_text(sec_text_layer, sec);
}

void deinit_canvas_layers(){
  layer_destroy(right_hours_layer);
  layer_destroy(left_hours_layer);
}

void deinit_time_text_layers(){
  text_layer_destroy(hour_text_layer);
  text_layer_destroy(min_text_layer);
  text_layer_destroy(sec_text_layer);
}


void window_load(){
  Layer * win_layer = window_get_root_layer(my_window);
  GRect win_bounds = layer_get_bounds(win_layer);
  GColor color;
  GColor bg_color;
  
  bg_color = (COLORS)? BACKGROUND_COLOR : GColorWhite;
  window_set_background_color(my_window, bg_color);
  
  w_center = GPoint(win_bounds.size.w/2, win_bounds.size.h/2);
    
  //add canvas that will hold everything
  canvas_layer = layer_create(GRect(0,5,win_bounds.size.w,win_bounds.size.h-10));
  layer_add_child(win_layer, canvas_layer);
  layer_set_update_proc(canvas_layer, my_update_proc);
  canvas_center = GPoint( win_bounds.size.w/2, (10+win_bounds.size.h)/2);
  
  //add battery text layer
  battery_text_layer = text_layer_create(GRect(win_bounds.size.w-30, 0, 30, 20));
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentRight);
  text_layer_set_background_color(battery_text_layer, bg_color);
  text_layer_set_font(battery_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(win_layer, (Layer *) battery_text_layer);
  
  //add battery img
  battery_img_layer = bitmap_layer_create(GRect(win_bounds.size.w-50,0,20,20));
  layer_add_child(win_layer, (Layer *) battery_img_layer);
  
  battery_handler(battery_state_service_peek());
  
  //add bluetooth img
  bluetooth_img_layer = bitmap_layer_create(GRect(win_bounds.size.w-70,0,20,20));
  layer_add_child(win_layer, (Layer *) bluetooth_img_layer);
  
  bluetooth_handler(bluetooth_connection_service_peek());
  
  //add date text layer
  date_text_layer = text_layer_create(GRect(0, 0, win_bounds.size.w-70, 20));
  text_layer_set_text_alignment(date_text_layer, GTextAlignmentLeft);
  text_layer_set_font(date_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  color = (COLORS)? HOURS_COLOR : GColorBlack;
  text_layer_set_text_color(date_text_layer, color);
  text_layer_set_background_color(date_text_layer, bg_color);
  layer_add_child(win_layer, text_layer_get_layer(date_text_layer));
  
  //add hour text layer
  hour_text_layer = text_layer_create(GRect(canvas_center.x-10, canvas_center.y-15, 20,25));
  text_layer_set_text(hour_text_layer, "H");
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  text_layer_set_font(hour_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  color = (COLORS)? HOURS_COLOR : GColorBlack;
  text_layer_set_text_color(hour_text_layer, color);
  text_layer_set_background_color(hour_text_layer, bg_color);
  layer_add_child(canvas_layer, text_layer_get_layer(hour_text_layer));

  //add min text layer
  min_text_layer = text_layer_create(GRect(canvas_center.x-10, canvas_center.y-(MIN_RADIUS+MIN_THICK/2+10), 20, 20));
  text_layer_set_text(min_text_layer, "M");
  text_layer_set_text_alignment(min_text_layer, GTextAlignmentCenter);
  text_layer_set_font(min_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  color = (COLORS)? MIN_COLOR : GColorBlack;
  text_layer_set_text_color(min_text_layer, color);
  text_layer_set_background_color(min_text_layer, bg_color);
  layer_add_child(canvas_layer, text_layer_get_layer(min_text_layer));

    //add sec text layer
  sec_text_layer = text_layer_create(GRect(canvas_center.x-10, canvas_center.y-(SEC_RADIUS+SEC_THICK+5), 20, 15));
  text_layer_set_text(sec_text_layer, "S");
  text_layer_set_text_alignment(sec_text_layer, GTextAlignmentCenter);
  text_layer_set_font(sec_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  color = (COLORS)? SEC_COLOR : GColorBlack;
  text_layer_set_text_color(sec_text_layer, color);
  text_layer_set_background_color(sec_text_layer, bg_color);
  layer_add_child(canvas_layer, text_layer_get_layer(sec_text_layer));
  

}

void window_unload(){
  text_layer_destroy(battery_text_layer);
  bitmap_layer_destroy(battery_img_layer);
  layer_destroy(canvas_layer);
  deinit_time_text_layers();
}

/*********************************** App **************************************/

void handle_init(void) {
  time_t t = time(NULL);
  struct tm *time_now = localtime(&t);
  tick_handler(time_now, SECOND_UNIT);
  
  my_window = window_create();

  window_set_window_handlers(my_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
                               
  window_stack_push(my_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  bluetooth_connection_service_subscribe(bluetooth_handler);
}

void handle_deinit(void) {
  window_destroy(my_window);
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
}

int main(void) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sec_R = %d, Min_R = %d, Hour_R = %d", SEC_RADIUS, MIN_RADIUS, HOURS_RADIUS);
  handle_init();
  app_event_loop();
  handle_deinit();
}
