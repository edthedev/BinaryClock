#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *hour_layer;
static TextLayer *minute_layer;
static TextLayer *second_layer;

static int hour_1;
static int hour_2;
static int hour_3;
static int hour_4;
static int hour_5;
static int hour_6;

static void to_binary_string(char* buffer, int number)
{
  hour_1 = (number) & 1;
  hour_2 = (number >> 1) & 1;
  hour_3 = (number >> 2) & 1;
  hour_4 = (number >> 3) & 1;
  hour_5 = (number >> 4) & 1;
  hour_6 = (number >> 5) & 1;
  snprintf(buffer, 7, "%d%d%d%d%d%d",hour_6,hour_5,hour_4,hour_3,hour_2,hour_1);
}

static void update_time()
{
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char sec_buffer[] = "000000";
  static char min_buffer[] = "000000";
  static char hour_buffer[] = "000000";

  to_binary_string(sec_buffer, tick_time->tm_sec);
  text_layer_set_text(second_layer, sec_buffer);
  
  to_binary_string(min_buffer, tick_time->tm_min);
  text_layer_set_text(minute_layer, min_buffer);
  
  to_binary_string(hour_buffer, tick_time->tm_hour);
  text_layer_set_text(hour_layer, hour_buffer);
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  // x, y, width, height
  hour_layer = text_layer_create(GRect(0, 0, 144, 50));
  minute_layer = text_layer_create(GRect(0, 40, 144, 50));
  second_layer = text_layer_create(GRect(0, 80, 144, 50));
  // text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(second_layer, GColorBlack);
  text_layer_set_text_color(minute_layer, GColorBlack);
  text_layer_set_text_color(hour_layer, GColorBlack);
  text_layer_set_text(second_layer, "sec");
  text_layer_set_text(minute_layer, "min");
  text_layer_set_text(hour_layer, "hour");

  // Improve the layout to be more like a watchface
  // text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_font(second_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_font(minute_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_font(hour_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(second_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(minute_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(hour_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minute_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hour_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(second_layer);
  text_layer_destroy(minute_layer);
  text_layer_destroy(hour_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

