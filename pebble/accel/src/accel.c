#include "pebble.h"

#define ACCEL_STEP_MS 1000

static Window *window;

static TextLayer *accel_layer;

static AppTimer *timer;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(accel_layer, "Move up");
}

/*static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(accel_layer, "Select pressed!");
}*/

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(accel_layer, "Move down");
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  //window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void timer_callback(void *data) {
  AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

  // Get AccelData in accel variable
  accel_service_peek(&accel);
  static char l[0x100];

  // Logic
  if((accel.x<500 && accel.x>-500) && accel.y>500){
	snprintf(l, sizeof(l), "Turn right");
  }else if((accel.x<500 && accel.x>-500) && accel.y<-500){
	snprintf(l, sizeof(l), "Turn left");
  }else if(accel.x>500 && (accel.y<500 && accel.y>-500)){
	snprintf(l, sizeof(l), "Go back");
  }else if(accel.x<-500 && (accel.y<500 && accel.y>-500)){
	snprintf(l, sizeof(l), "Go ahead");
  }else if(accel.x>500 && accel.y<-500){
	snprintf(l, sizeof(l), "Go back and left");
  }else if(accel.x<-500 && accel.y<-500){
	snprintf(l, sizeof(l), "Go ahead and left");
  }else if(accel.x>500 && accel.y>500){
	snprintf(l, sizeof(l), "Go back and right");
  }else if(accel.x<-500 && accel.y>500){
	snprintf(l, sizeof(l), "Go ahead and right");
  }else{
	snprintf(l, sizeof(l), "Stay");
  }
  text_layer_set_text(accel_layer, l);

  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  accel_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  text_layer_set_font(accel_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(accel_layer, "No data yet.");
  text_layer_set_overflow_mode(accel_layer, GTextOverflowModeWordWrap);
  text_layer_set_background_color(accel_layer, GColorWhite);
  text_layer_set_text_color(accel_layer, GColorBlack);
  text_layer_set_text_alignment(accel_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(accel_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(accel_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(window, true /* Animated */);

  accel_data_service_subscribe(0, NULL);
  window_set_click_config_provider(window, click_config_provider);
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void deinit(void) {
  accel_data_service_unsubscribe();

  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
