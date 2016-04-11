#ifndef ORMQT_GLOBAL_H
#define ORMQT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ORMQT_LIBRARY)
#  define ORMQTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ORMQTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ORMQT_GLOBAL_H
