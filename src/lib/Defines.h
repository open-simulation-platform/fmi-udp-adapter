#ifndef DEFINES_H
#define DEFINES_H

#include <QtCore/qglobal.h>

#if defined(EXPORT_DLL)
#  define FMI2_Export Q_DECL_EXPORT
#  define DECLSPEC Q_DECL_EXPORT
#else
#  define FMI2_Export Q_DECL_IMPORT
#  define DECLSPEC Q_DECL_IMPORT
#endif

#endif // DEFINES_H
