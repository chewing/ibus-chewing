#include "MakerDialogUtil.h"
#include "MakerDialogBackend.h"

MkdgBackend *mkdg_backend_new(const gchar * id, gpointer config,
			      const gchar * basePath, gpointer auxData)
{
    g_assert(config);
    g_assert(!STRING_IS_EMPTY(id));
    MkdgBackend *result = g_new0(MkdgBackend, 1);
    result->id = id;
    result->config = config;
    result->basePath = basePath;
    result->auxData = auxData;
    result->flags = 0;
    return result;
}

gchar *mkdg_backend_get_key(MkdgBackend * backend,
			    const gchar * section, const gchar * key,
			    gpointer userData)
{
    return backend->getKeyFunc(backend, section, key, userData);
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
