#include <ibusservice.h>
#include <ibusconfig.h>

typedef struct{
    IBusConfig *config;
    MakerDialog *settingDialog;
} IBusChewingConfig;

enum IBUS_CHEWING_CONFIG_FLAG{
    IBUS_CHEWING_CONFIG_FLAG_INVISIBLE   =0x1,
    IBUS_CHEWING_CONFIG_FLAG_INSENSITIVE =0x2,
    IBUS_CHEWING_CONFIG_FLAG_INEDITABLE  =0x4,
    IBUS_CHEWING_CONFIG_FLAG_HAS_TRANSLATION =0x8,
    IBUS_CHEWING_CONFIG_FLAG_TRANSLATION_WITH_CONTEXT =0x10,
} Maker:Dialog:Property:Flag;

typedef struct{
    GType valueType;
    gchar key[30];
    gchar pageName[50];
    gchar label[200];
    gchar defaultValue[100];
    const gchar **validValues;
    gchar *translationContext;

    gint min;
    gint max;

    CallbackGetFunc getFunc;
    CallbackSetFunc setFunc;

    MakerDialogPropertyFlags propertyFlags;
    gint width;
    gint height;
    const gchar *tooltip;
    gpointer userData;
} IBusChewingConfigPropertySpec;

IBusChewingConfig *IBusChewingConfig_new(IBusService *service, MakerDialog *settingDialog);

