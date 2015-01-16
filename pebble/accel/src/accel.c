#include "pebble.h"

//#define MATH_PI 3.141592653589793238462
//#define NUM_DISCS 20
//#define DISC_DENSITY 0.25
//#define ACCEL_RATIO 0.05
#define ACCEL_STEP_MS 1000
/*
typedef struct Vec2d {
  double x;
  double y;
} Vec2d;

typedef struct Disc {
  Vec2d pos;
  Vec2d vel;
  double mass;
  double radius;
} Disc;
*/
//static Disc discs[NUM_DISCS];

//static double next_radius = 3;

static Window *window;

static TextLayer *accel_layer;

//static GRect window_frame;

//static Layer *disc_layer;

static AppTimer *timer;
/*
static double disc_calc_mass(Disc *disc) {
  return MATH_PI * disc->radius * disc->radius * DISC_DENSITY;
}

static void disc_init(Disc *disc) {
  GRect frame = window_frame;
  disc->pos.x = frame.size.w/2;
  disc->pos.y = frame.size.h/2;
  disc->vel.x = 0;
  disc->vel.y = 0;
  disc->radius = next_radius;
  disc->mass = disc_calc_mass(disc);
  next_radius += 0.5;
}

static void disc_apply_force(Disc *disc, Vec2d force) {
  disc->vel.x += force.x / disc->mass;
  disc->vel.y += force.y / disc->mass;
}

static void disc_apply_accel(Disc *disc, AccelData accel) {
  Vec2d force;
  force.x = accel.x * ACCEL_RATIO;
  force.y = -accel.y * ACCEL_RATIO;
  disc_apply_force(disc, force);
}

static void disc_update(Disc *disc) {
  const GRect frame = window_frame;
  double e = 0.5;
  if ((disc->pos.x - disc->radius < 0 && disc->vel.x < 0)
    || (disc->pos.x + disc->radius > frame.size.w && disc->vel.x > 0)) {
    disc->vel.x = -disc->vel.x * e;
  }
  if ((disc->pos.y - disc->radius < 0 && disc->vel.y < 0)
    || (disc->pos.y + disc->radius > frame.size.h && disc->vel.y > 0)) {
    disc->vel.y = -disc->vel.y * e;
  }
  disc->pos.x += disc->vel.x;
  disc->pos.y += disc->vel.y;
}

static void disc_draw(GContext *ctx, Disc *disc) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(disc->pos.x, disc->pos.y), disc->radius);
}

static void disc_layer_update_callback(Layer *me, GContext *ctx) {
  for (int i = 0; i < NUM_DISCS; i++) {
    disc_draw(ctx, &discs[i]);
  }
}
*/

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
  
  //text_update(accel);
  //static char sx[0x100];
  static char l[0x100];

  // convert 123 to string [buf]
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
  //snprintf(sx, sizeof(sx), "x = %d; y = %d; z = %d", accel.x, accel.y, accel.z);
  text_layer_set_text(accel_layer, l);
  /*for (int i = 0; i < NUM_DISCS; i++) {
    Disc *disc = &discs[i];
    disc_apply_accel(disc, accel);
    disc_update(disc);
  }*/

  //layer_mark_dirty(accel_layer);

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
  
  //layer_set_update_proc(accel_layer, disc_layer_update_callback);
  layer_add_child(window_layer, text_layer_get_layer(accel_layer));

  
  /*for (int i = 0; i < NUM_DISCS; i++) {
    disc_init(&discs[i]);
  }*/
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
  //window_set_background_color(window, GColorBlack);

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
