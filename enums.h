#ifndef ENUMS_H
#define ENUMS_H

#include <qobjectdefs.h>

namespace Enums
{
	Q_NAMESPACE
	enum class DummyEnumType
	{
		Option1,
		Option2
	};
	Q_ENUM_NS(DummyEnumType)
}

#endif // ENUMS_H
