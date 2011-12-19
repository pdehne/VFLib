// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

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
