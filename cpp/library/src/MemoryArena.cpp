//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Common (finjin-common).
//
//Finjin Common is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/common/MemoryArena.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/MemorySize.hpp"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------

//MemoryArenaSettings
MemoryArenaSettings::MemoryArenaSettings()
{
    this->byteCount = 0;
    this->alignment = 0;
}

void MemoryArenaSettings::ParseSettings(const ByteBufferReader& configFileBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (section.empty())
                {
                    if (key == "size")
                    {
                        MemorySize::Parse(this->byteCount, value.ToString(), error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'size'.", value));
                            return;
                        }
                    }
                    else if (key == "alignment")
                    {
                        this->alignment = Convert::ToInteger(value.ToString(), this->alignment);
                    }
                }

                break;
            }
            default: break;
        }
    }
}
