#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0xDF, 0x3C, 0xF9, 0x8C, 0x36, 0xBC, 0x4E, 0x9A, 0xA0, 0x5E, 0xAC, 0x9D, 0x7D, 0xC5, 0x97, 0x7E}
PBL_APP_INFO(MY_UUID, "Triforce Clock", "saulricardo", 2,1 /* App version */,DEFAULT_MENU_ICON,APP_INFO_WATCH_FACE);


Window window;

TextLayer timeLayer; //Clock
TextLayer text_date_layer; //Date
TextLayer AMPM; //AM/PM

RotBmpPairContainer triforce; //Image

RotBmpPairContainer dots;
int32_t current_layer_angle = 0;

void update_time_display() {
  static char timeText[] = "00:00";
  static char date_text[] = "Xxx 00";
  
  char *timeFormat;

  PblTm currentTime;

  GContext* ctx = app_get_current_graphics_context();

    timeFormat = "%I:%M";
  //get pebble Time
  get_time(&currentTime);

  string_format_time(date_text, sizeof(date_text), "%b %e", &currentTime);
  text_layer_set_text(&text_date_layer, date_text);

  string_format_time(timeText, sizeof(timeText), timeFormat, &currentTime);

  if (!clock_is_24h_style() && (timeText[0] == '0')) {
    memmove(timeText, &timeText[1], sizeof(timeText) - 1);
  }
  //Set PM or AM 
	int hour = currentTime.tm_hour;
	if(hour > 12){
	text_layer_set_text(&AMPM, "PM");
	} else {
	text_layer_set_text(&AMPM, "AM");
	}
  text_layer_set_text(&timeLayer, timeText);
  
    // This implementation assumes only one type of timer used in the app.

  current_layer_angle = (current_layer_angle + 10) % 360;

  // This will automatically mark the layer dirty and update it.
  rotbmp_pair_layer_set_angle(&dots.layer, TRIG_MAX_ANGLE * current_layer_angle / 360);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Triforce");
  window_stack_push(&window, true /*animated */);
  window_set_background_color(&window,GColorBlack);
    
  // Display the black and white image with transparency.
  resource_init_current_app(&APP_RESOURCES);

  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_TRIFORCE_WHITE, RESOURCE_ID_IMAGE_TRIFORCE_BLACK, &triforce);
  triforce.layer.layer.frame.origin.x = 5;
  triforce.layer.layer.frame.origin.y = -10;
  
  layer_add_child(&window.layer, &triforce.layer.layer);
  
  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_DOTS_WHITE, RESOURCE_ID_IMAGE_DOTS_BLACK, &dots);
  dots.layer.layer.frame.origin.x = 5;
  dots.layer.layer.frame.origin.y = -10;
  
  layer_add_child(&window.layer, &dots.layer.layer);
  	
  //Initializes the timeLayer  
  text_layer_init(&timeLayer, GRect(0, 168-42, 144 /* width */, 42 /* height */));
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
  text_layer_set_text_color(&timeLayer, GColorWhite);
  layer_set_frame(&timeLayer.layer, GRect(0, 100, 144, 168-100));
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRIFORCE_30)));

  layer_add_child(&window.layer, &timeLayer.layer);
  
  //Initializes the Date layer
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(40, 140, 144-40, 168-140));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRIFORCE_20)));
  layer_add_child(&window.layer, &text_date_layer.layer);
  
  //Initializes the AM/PM layer
  text_layer_init(&AMPM, window.layer.frame);
  text_layer_set_text_color(&AMPM, GColorWhite);
  text_layer_set_background_color(&AMPM, GColorClear);
  layer_set_frame(&AMPM.layer, GRect(110, 107, 144-110, 168-107));
  layer_add_child(&window.layer, &AMPM.layer);

  update_time_display();

}
void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  rotbmp_pair_deinit_container(&triforce);
  rotbmp_pair_deinit_container(&dots);
}

// Called once per second
static void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;
  (void)t;
  update_time_display();
}
//Main Loop
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
        // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
