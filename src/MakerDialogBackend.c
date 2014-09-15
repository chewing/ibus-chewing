#include "MakerDialogBackend.h"

MkdgBackend *mkdg_backend_new(gpointer config, const gchar * baseDir,
			      gpointer auxData)
{
    if (config == NULL) {
	return NULL;
    }
    MkdgBackend *result = g_new0(MkdgBackend, 1);
    result->config = config;
    result->baseDir = baseDir;
    result->auxData = auxData;
    return result;
}

GValue *mkdg_backend_read(MkdgBackend * backend, GValue * value,
			  const gchar * section, const gchar * key,
			  gpointer userData)
{
    return backend->readFunc(backend, value, section, key, userData);
}

gboolean mkdg_backend_write(MkdgBackend * backend, GValue * value,
			    const gchar * section, const gchar * key,
			    gpointer userData)
{
    return backend->writeFunc(backend, value, section, key, userData);
}
