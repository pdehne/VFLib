// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef __VF_DB_DETAIL_ERROR_CODES_VFHEADER__
#define __VF_DB_DETAIL_ERROR_CODES_VFHEADER__

namespace db {

namespace detail {

// Converts SQLITE error codes into an Error object
extern const Error sqliteError (const char* sourceFileName,
                                int lineNumber,
                                int sqliteErrorCode);
}

}

#endif
