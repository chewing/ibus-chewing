#include "MakerDialogBackend.h"

static MkdgBackend *mkdgBackend = NULL;

MkdgBackend *mkdg_backend_new(gpointer config, gpointer auxData)
{
    if (config == NULL) {
	return NULL;
    }
    MkdgBackend *result = g_new0(MkdgBackend, 1);
    result->obj = obj;
    result->auxData = auxData;
    return result;
}

GValue *mkdg_value_load(MkdgBackend * backend, GValue * value,
			const gchar * section, const gchar * key)
{
    return backend->load_value(backend, value, section, key);
}

gboolean mkdg_value_save(MkdgBackend * backend, GValue * value,
			 const gchar * section, const gchar * key)
{
    return backend->load_value(backend, value, section, key);
}

