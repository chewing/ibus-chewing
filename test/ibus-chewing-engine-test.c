#include <glib.h>
#include "ibus-chewing-engine.h"
#include "ibus-chewing-engine-private.h"
static IBusChewingEngine *engine = NULL;

IBusChewingEngine *ibus_chewing_engine_new()
{
    return (IBusChewingEngine *) g_object_new(IBUS_TYPE_CHEWING_ENGINE,
					      NULL);
}

void xim_focus_out_then_focus_in_test()
{
    ibus_chewing_engine_focus_in(engine);
    ibus_chewing_engine_enable(engine);
//    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 0x75, 0x16,
//					  IBUS_RELEASE_MASK);
//    ibus_chewing_engine_focus_out(engine);
//    ibus_chewing_engine_focus_in(engine);
//    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 0x75, 0x16,
//					  IBUS_RELEASE_MASK);
}


gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
    engine = ibus_chewing_engine_new();
    ibus_chewing_engine_set_status_flag(engine, ENGINE_FLAG_STANDALONE);

    g_test_add_func
	("/ibus-chewing/ibus-chewing-engine/xim_focus_out_then_focus_in_test",
	 xim_focus_out_then_focus_in_test);
    return g_test_run();
}
