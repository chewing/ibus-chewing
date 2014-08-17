#include "IBusChewingConfig.h"

IBusChewingConfig *IBusChewingConfig_new(IBusService *service,MakerDialog *settingDialog){
    IBusConfig *config=NULL;
#if IBUS_CHECK_VERSION(1, 4, 0)
    GError *error=NULL;
    GDBusConnection *connection=ibus_service_get_connection(*service);
    config=g_object_ref_sink(ibus_config_new (connection, NULL, &error));
    g_assert(error==NULL);
#else
    GList  *connections_list=ibus_service_get_connections(IBusService *service);
    g_assert(connections_list);
    g_assert(connections_list->data);
    IBusConnection *iConnection=(IBusConnection *) connections_list->data;
    config=g_object_ref_sink(ibus_config_new(iConnection));
#endif
    if (config){
	IBusChewingConfig *result=g_new0(IBusChewingConfig, 1);
	result->config=config;
	result->settingDialog=settingDialog;
	return result;
    }
    return NULL;
}

gboolean IBusChewingConfig_get_value(IBusChewingConfig *self, const gchar *section, const gchar *key, GValue *gValue){
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar=ibus_config_get_value(self->config, section, key);
    if (gVar==NULL){
	return FALSE;
    }
    g_variant_ref_sink(gVar);
    g_variant_to_g_value(gVar, gValue);
    g_variant_unref(gVar);
    return TRUE;
#else
    return ibus_config_get_value(self->config, section, key, gValue);
#endif
}

gboolean IBusChewingConfig_set_value(IBusChewingConfig *self, const gchar *section, const gchar *key, GValue *gValue){
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar=g_variant_ref_sink(g_value_to_g_variant(gValue));
    if (gVar!=NULL){
	return ibus_config_set_value(self->config, section, key, gVar);
    }else{
	return FALSE;
    }
#else
    return ibus_config_set_value(self->config, section, key, gValue);
#endif
}

/**
 * IBusChewingConfig_save:
 * @self: this instances.
 * @keySuffix: key to be set.
 * @returns: TRUE if successful, FALSE otherwise.
 *
 * Save the property value to disk.
 */
gboolean IBusChewingConfig_save(IBusChewingConfig *self, const gchar *keySuffix){
    IBUS_CHEWING_LOG(1,"[I1] IBusChewingConfig_save(%s,-)",keySuffix);
    GValue gValue={0};
    maker_dialog_get_widget_value(MAKER_DIALOG(self->settingDialog),keySuffix,&gValue);
    return ibus_chewing_config_set_value (self->config, "engine/Chewing", keySuffix,&gValue);
}

/**
 * IBusChewingConfig_save_all:
 * @self: this instances.
 * @returns: TRUE if all successful, FALSE otherwise.
 *
 * Save alll property values to disk.
 */
public gboolean IBusChewingConfig_save_all(IBusChewingConfig *self){
    int i;
    gboolean success=TRUE;
    for(i=0;propSpecs[i].valueType!=G_TYPE_INVALID;i++){
	if (!self_IBusChewingConfig_save(self,propSpecs[i].key)){
	    success=FALSE;
	}
    }
    return success;
}


