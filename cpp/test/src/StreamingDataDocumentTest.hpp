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


//Includes---------------------------------------------------------------------
#include "finjin/common/TextDataChunkReader.hpp"
#include "finjin/common/TextDataChunkWriter.hpp"
#include "finjin/common/JsonDataChunkReader.hpp"
#include "finjin/common/JsonDataChunkWriter.hpp"
#include "finjin/common/BinaryDataChunkReader.hpp"
#include "finjin/common/BinaryDataChunkWriter.hpp"
#include "finjin/common/DataChunkWriterController.hpp"
#include "finjin/common/DataChunkReaderController.hpp"
#include "finjin/common/DataChunkReaderCallbacks.hpp"
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StandardPaths.hpp"
#include <iostream>
#include <future>
#include <list>
#include <fstream>

#define TEST_BYTES 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49

struct ChunkNames
{
    static const ChunkName MESH;
    static const ChunkName VERTICES;
    static const ChunkName SUBMESHES;
};
const ChunkName ChunkNames::MESH("mesh", "2C604A04-7F50-4090-816E-8C3A62B40AC2");
const ChunkName ChunkNames::VERTICES("verts", "79E9C114-2780-4840-94F1-7D8F19F9AF6B");
const ChunkName ChunkNames::SUBMESHES("submeshes", "3B5E79FC-BCBC-49CB-9F68-BACF447EA304");

struct ChunkPropertyNames
{
    static const ChunkPropertyName CREATED;
    static const ChunkPropertyName ID;
    static const ChunkPropertyName NAME;
    static const ChunkPropertyName VERTEX_COUNT;
    static const ChunkPropertyName FREE_TEXT;
    static const ChunkPropertyName EMBEDDED_IMAGE;
    static const ChunkPropertyName EMBEDDED_QUOTE_TEXT;
    static const ChunkPropertyName NEWLINE_TEXT;
    static const ChunkPropertyName OPEN_CURLY;
    static const ChunkPropertyName CLOSE_CURLY;
    static const ChunkPropertyName DOUBLES;
    static const ChunkPropertyName INCLUDE_FILE;
    static const ChunkPropertyName VERTEX_POSITIONS;
    static const ChunkPropertyName VERTEX_NORMALS;
    static const ChunkPropertyName VERTEX_TIMES;
    static const ChunkPropertyName VERTEX_BOOLS;
    static const ChunkPropertyName VERTEX_STRINGS;
    static const ChunkPropertyName VERTEX_MATRICES;
    static const ChunkPropertyName VERTEX_SIZES;
};
const ChunkPropertyName ChunkPropertyNames::CREATED("created");
const ChunkPropertyName ChunkPropertyNames::ID("id");
const ChunkPropertyName ChunkPropertyNames::NAME("name");
const ChunkPropertyName ChunkPropertyNames::VERTEX_COUNT("vertex-count");
const ChunkPropertyName ChunkPropertyNames::FREE_TEXT("free-text");
const ChunkPropertyName ChunkPropertyNames::EMBEDDED_IMAGE("embedded-image");
const ChunkPropertyName ChunkPropertyNames::EMBEDDED_QUOTE_TEXT("embedded-quote-text");
const ChunkPropertyName ChunkPropertyNames::NEWLINE_TEXT("newline-text");
const ChunkPropertyName ChunkPropertyNames::OPEN_CURLY("open-curly");
const ChunkPropertyName ChunkPropertyNames::CLOSE_CURLY("close-curly");
const ChunkPropertyName ChunkPropertyNames::DOUBLES("doubles");
const ChunkPropertyName ChunkPropertyNames::INCLUDE_FILE("include-file");
const ChunkPropertyName ChunkPropertyNames::VERTEX_POSITIONS("vertex-positions");
const ChunkPropertyName ChunkPropertyNames::VERTEX_NORMALS("vertex-normals");
const ChunkPropertyName ChunkPropertyNames::VERTEX_TIMES("vertex-times");
const ChunkPropertyName ChunkPropertyNames::VERTEX_BOOLS("vertex-bools");
const ChunkPropertyName ChunkPropertyNames::VERTEX_STRINGS("vertex-strings");
const ChunkPropertyName ChunkPropertyNames::VERTEX_MATRICES("vertex-matrices");
const ChunkPropertyName ChunkPropertyNames::VERTEX_SIZES("vertex-sizes");

struct Vertex
{
    float pos[3];
    float normal[3];
    TimeDuration t;
    bool b;
    Utf8String s;
    float matrix[4][4];
    size_t size;
};

static Vertex verts[] =
{
    { { 1, 2, 3 },{ 4, 5, 6 }, TimeDuration::Seconds(1), false, "a",{ { 0, 0.01f, 0.02f, 0.03f },{ 0.04f, 0.05f, 0.06f, 0.07f },{ 0.08f, 0.09f, 0.10f, 0.11f },{ 0.12f, 0.13f, 0.14f, 0.15f } }, 123 },
    { { 11, 12, 13 },{ 14, 15, 16 }, TimeDuration::Seconds(2), true, "b",{ { 1, 1.01f, 1.02f, 1.03f },{ 1.04f, 1.05f, 1.06f, 1.07f },{ 1.08f, 1.09f, 1.10f, 1.11f },{ 1.12f, 1.13f, 1.14f, 1.15f } }, 345 },
    { { 111, 112, 113 },{ 114, 115, 116 }, TimeDuration::Seconds(3), false, "c",{ { 2, 2.01f, 2.02f, 2.03f },{ 2.04f, 2.05f, 2.06f, 2.07f },{ 2.08f, 2.09f, 2.10f, 2.11f },{ 2.12f, 2.13f, 2.14f, 2.15f } }, 456 },
    { { 1111, 1112, 1113 },{ 1114, 1115, 1116 }, TimeDuration::Seconds(4), true, "d",{ { 3, 3.01f, 3.02f, 3.03f },{ 3.04f, 3.05f, 3.06f, 3.07f },{ 3.08f, 3.09f, 3.10f, 3.11f },{ 3.12f, 3.13f, 3.14f, 3.15f } }, 567 },
    { { 11111, 11112, 11113 },{ 11114, 11115, 11116 }, TimeDuration::Seconds(5), false, "e",{ { 4, 4.01f, 4.02f, 4.03f },{ 4.04f, 4.05f, 4.06f, 4.07f },{ 4.08f, 4.09f, 4.10f, 1.11f },{ 4.12f, 4.13f, 4.14f, 4.15f } }, 789 },
    { { 111111, 111112, 111113 },{ 111114, 111115, 111116 }, TimeDuration::Seconds(6), true, "f",{ { 5, 5.01f, 5.02f, 5.03f },{ 5.04f, 5.05f, 5.06f, 5.07f },{ 5.08f, 5.09f, 5.10f, 5.11f },{ 5.12f, 5.13f, 5.14f, 5.15f } }, 147 }
};
static int vertexCount = sizeof(verts) / sizeof(verts[0]);


//MultipleTextFileDataChunkWriterController------------------
class MultipleTextFileDataChunkWriterController : public DataChunkWriterController
{
public:
    MultipleTextFileDataChunkWriterController(const Path& outputDirectory);

    std::unique_ptr<DataChunkWriter> CreateWriter(std::shared_ptr<DocumentWriterOutput> outPtr, Error& error) override;

    bool RequiresNewOutput(const DataChunkWriter& writer, const ChunkName& chunkName) const override;
    std::unique_ptr<DocumentWriterOutput> AddOutput(DataChunkWriter& writer, const ChunkName& chunkName, Error& error) override;
    bool ScheduleWriteChunk(std::shared_ptr<DataChunkWriter> writer, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error) override;

    void FinishScheduledWork() override;

private:
    Path outputDirectory;
    int counter;
    std::list<std::future<void>> scheduledChunkWrites;
};

MultipleTextFileDataChunkWriterController::MultipleTextFileDataChunkWriterController(const Path& _outputDirectory) : outputDirectory(_outputDirectory)
{
    this->counter = 0;
}

std::unique_ptr<DataChunkWriter> MultipleTextFileDataChunkWriterController::CreateWriter(std::shared_ptr<DocumentWriterOutput> outPtr, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    JsonDataChunkWriter::Settings settings;
    settings.Create(outPtr, *this);

    //std::unique_ptr<DataChunkWriter> writer(new TextDataChunkWriter(outPtr, *this));
    std::unique_ptr<JsonDataChunkWriter> jsonWriter(new JsonDataChunkWriter);
    jsonWriter->CreateRoot(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create writer.");
        return nullptr;
    }

    std::unique_ptr<DataChunkWriter> writer(jsonWriter.release());
    return writer;
}

bool MultipleTextFileDataChunkWriterController::RequiresNewOutput(const DataChunkWriter& writer, const ChunkName& chunkName) const
{
    return chunkName == ChunkNames::MESH;
}

std::unique_ptr<DocumentWriterOutput> MultipleTextFileDataChunkWriterController::AddOutput(DataChunkWriter& writer, const ChunkName& chunkName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto fileName = Utf8StringFormatter::Format("%1%_%2%.txt", chunkName.name, this->counter++);
    auto fileUri = fileName;// "file:///" + fileName;

    writer.WriteString(ChunkPropertyNames::INCLUDE_FILE, fileUri, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to write 'include-file' string.");
        return nullptr;
    }
    else
    {
        auto fullFilePath = this->outputDirectory;
        fullFilePath /= fileName;
        auto outputStream = CreateOutputFileStream(fullFilePath, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add output '%1%' for chunk '%2%'.", fullFilePath, chunkName.name));
        return outputStream;
    }
}

bool MultipleTextFileDataChunkWriterController::ScheduleWriteChunk(std::shared_ptr<DataChunkWriter> writer, std::function<void(DataChunkWriter&, Error&)> chunkFunc, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::shared_ptr<DataChunkWriter> sharedWriter(writer);

    this->scheduledChunkWrites.push_back(std::async(std::launch::async, [=] 
    {
        FINJIN_DECLARE_ERROR(error);

        chunkFunc(*sharedWriter, error);

        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }));

    return true;
}

void MultipleTextFileDataChunkWriterController::FinishScheduledWork()
{
    for (auto& f : this->scheduledChunkWrites)
        f.get(); //TODO: Should I wrap f.get() in a try/catch?
    this->scheduledChunkWrites.clear();
}


//MultipleTextFileDataChunkReaderController------------------
class MultipleTextFileDataChunkReaderController : public DataChunkReaderController
{
public:
    MultipleTextFileDataChunkReaderController(const Path& inputDirectory);

    Path ResolveInputFilePath(const Path& filePath) override;

    std::unique_ptr<DataChunkReader> CreateReader(std::shared_ptr<DataChunkReaderInput> inPtr, Error& error) override;

    void ScheduleReadChunk(std::function<void(DataChunkReader&, Error&)> chunkFunc, std::unique_ptr<DataChunkReader> reader, Error& error) override;

    void FinishScheduledWork() override;

private:
    Path inputDirectory;
    std::list<std::future<void>> scheduledChunkReads;
};

MultipleTextFileDataChunkReaderController::MultipleTextFileDataChunkReaderController(const Path& _inputDirectory) : inputDirectory(_inputDirectory)
{
}

Path MultipleTextFileDataChunkReaderController::ResolveInputFilePath(const Path& filePath)
{
    if (Path(filePath).IsAbsolute())
        return filePath;
    else
    {
        Path result;
        result = this->inputDirectory;
        result /= filePath;
        return result;
    }
}

std::unique_ptr<DataChunkReader> MultipleTextFileDataChunkReaderController::CreateReader(std::shared_ptr<DataChunkReaderInput> inPtr, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    JsonDataChunkReader::Settings readerSettings;
    readerSettings.Create(inPtr, *this);
    
    //std::unique_ptr<DataChunkReader> reader(new TextDataChunkReader(readerSettings, inPtr, *this));
    std::unique_ptr<JsonDataChunkReader> jsonReader(new JsonDataChunkReader);
    jsonReader->Create(readerSettings, nullptr, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create reader.");
        return nullptr;
    }

    std::unique_ptr<DataChunkReader> reader(jsonReader.release());
    return reader;
}

void MultipleTextFileDataChunkReaderController::ScheduleReadChunk(std::function<void(DataChunkReader&, Error&)> chunkFunc, std::unique_ptr<DataChunkReader> reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::shared_ptr<DataChunkReader> sharedReader(reader.release());

    this->scheduledChunkReads.push_back(std::async(std::launch::async, [=] {
        FINJIN_DECLARE_ERROR(error);

        chunkFunc(*sharedReader, error);

        if (error)
            FINJIN_SET_ERROR_NO_MESSAGE(error);
    }));
}

void MultipleTextFileDataChunkReaderController::FinishScheduledWork()
{
    for (auto& f : this->scheduledChunkReads)
        f.get(); //TODO: Should I wrap f.get() in a try/catch?
    this->scheduledChunkReads.clear();
}


//Main---------------------
void MultifileWriteTest(DataChunkWriter& writer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    writer.WriteWriterHeader(error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    writer.WriteDateTime(ChunkPropertyNames::CREATED, DateTime::NowUtc(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    writer.WriteUuid(ChunkPropertyNames::ID, Uuid::CreateRandom(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    writer.WriteChunk(ChunkNames::MESH, [](DataChunkWriter& meshFormatWriter, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        meshFormatWriter.WriteString(ChunkPropertyNames::NAME, "Human", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteInt32(ChunkPropertyNames::VERTEX_COUNT, 40, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::FREE_TEXT, "This\nIs\nA\nLot\nOf\nLines", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::EMBEDDED_QUOTE_TEXT, "->\"<- ->'<-", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::OPEN_CURLY, "{", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::CLOSE_CURLY, "}", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteChunk(ChunkNames::VERTICES, [](DataChunkWriter& vertsFormatWriter, Error& error2)
        {
            FINJIN_ERROR_METHOD_START(error2);

            double d[2] = { 1.5, 2.5 };
            vertsFormatWriter.WriteDoubles(ChunkPropertyNames::DOUBLES, d, 2, error2);
            vertsFormatWriter.WriteDoubles(ChunkPropertyNames::DOUBLES, d, 2, error2);
            vertsFormatWriter.WriteDoubles(ChunkPropertyNames::DOUBLES, d, 2, error2);
            if (error2)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error2);
                return;
            }
        }, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }
    }, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

void MultifileReadTest(DataChunkReader& reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    DataHeader dataHeader;
    reader.ReadReaderHeader(dataHeader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read reader header.");
        return;
    }
    else
    {
        std::cout << "Format = " << reader.GetHeader().format << std::endl;
        std::cout << "Version = " << reader.GetHeader().version << std::endl;
        std::cout << "-----------------------------------------------------" << std::endl;

        bool reading = true;
        while (reading)
        {
            reader.ReadDataHeader(dataHeader, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read data header.");
                reading = false;
            }
            else
            {
                switch (dataHeader.type)
                {
                    case DataHeaderType::CHUNK_START:
                    {
                        std::cout << "Encountered chunk start: " << dataHeader.name << std::endl;
                        break;
                    }
                    case DataHeaderType::CHUNK_END:
                    {
                        std::cout << "Encountered chunk end." << std::endl;
                        break;
                    }
                    case DataHeaderType::PROPERTY:
                    {
                        ParsedChunkPropertyName key;
                        Utf8String value;
                        reader.ReadPropertyName(dataHeader, key, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to read key.");
                            reading = false;
                        }
                        else
                        {
                            if (key == ChunkPropertyNames::INCLUDE_FILE)
                            {
                                reader.ReadString(dataHeader, value, error);
                                if (error)
                                {
                                    FINJIN_SET_ERROR(error, "Failed to read string value.");
                                    reading = false;
                                }
                                else
                                {
                                    std::cout << "Including file: " << value << std::endl;

                                    reader.GetReaderController().ReadInputFile(value, [](DataChunkReader& meshFormatReader, Error& error) 
                                    {
                                        FINJIN_ERROR_METHOD_START(error);

                                        std::cout << "Reading include file chunk" << std::endl;
                                    }, error);
                                    if (error)
                                    {
                                        FINJIN_SET_ERROR(error, "Failed to read input file for include.");
                                        reading = false;
                                    }
                                }
                            }
                            else
                            {
                                reader.ReadString(dataHeader, value, error);
                                std::cout << key.ToString() << " = " << value << std::endl;
                            }
                        }

                        break;
                    }
                    case DataHeaderType::END:
                    {
                        std::cout << "Encountered end." << std::endl;
                        reading = false;
                        break;
                    }
                }
            }
        }
    }
}

//Tests------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(StreamingDataDocumentTest_create_files)
{
    BOOST_TEST_MESSAGE("StreamingDataDocumentTest_create_files:");

    FINJIN_DECLARE_ERROR(error);

    StandardPaths standardPaths;
    standardPaths.Create("", nullptr, error);
    BOOST_CHECK(!error);

    RandomUuidCreator uuidCreator;

    standardPaths.userApplicationTemporaryDirectory.CreateDirectories();
    Path outputDirectory = standardPaths.userApplicationTemporaryDirectory.path;
    Path outputFilePath = outputDirectory;
    outputFilePath /= "exported.fstd";

    std::cout << "Output directory: " << outputDirectory << std::endl;
    std::cout << "Output file: " << outputFilePath << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    {
        //Write
        MultipleTextFileDataChunkWriterController writerController(outputDirectory);
        writerController.WriteOutputFile(outputFilePath, MultifileWriteTest, error);
        if (!error)
            writerController.FinishScheduledWork();
    }
    if (error)
    {
        std::cout << error.ToString() << std::endl;
        BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
        BOOST_CHECK(!error);
    }
    else
    {
        //Read
        MultipleTextFileDataChunkReaderController readerController(outputDirectory);
        readerController.ReadInputFile(outputFilePath, MultifileReadTest, error);
        if (!error)
            readerController.FinishScheduledWork();

        if (error)
        {
            std::cout << error.ToString() << std::endl;
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
            BOOST_CHECK(!error);
        }
    }
}

void InMemoryWriteTest(DataChunkWriter& writer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    writer.WriteWriterHeader(error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    writer.WriteDateTime(ChunkPropertyNames::CREATED, DateTime::NowUtc(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    writer.WriteUuid(ChunkPropertyNames::ID, Uuid::CreateRandom(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    writer.WriteChunk(ChunkNames::MESH, [](DataChunkWriter& meshFormatWriter, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        meshFormatWriter.WriteString(ChunkPropertyNames::NAME, "Human", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteCount(ChunkPropertyNames::VERTEX_COUNT, vertexCount, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteStridedFloats(ChunkPropertyNames::VERTEX_POSITIONS, &verts[0].pos[0], vertexCount * 3, DataChunkWriteStride(3, sizeof(Vertex)), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteStridedTimeDurations(ChunkPropertyNames::VERTEX_TIMES, &verts[0].t, vertexCount, DataChunkWriteStride(1, sizeof(Vertex)), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteStridedBools(ChunkPropertyNames::VERTEX_BOOLS, &verts[0].b, vertexCount, DataChunkWriteStride(1, sizeof(Vertex)), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteStridedStrings(ChunkPropertyNames::VERTEX_STRINGS, &verts[0].s, vertexCount, DataChunkWriteStride(1, sizeof(Vertex)), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteStridedFloats(ChunkPropertyNames::VERTEX_MATRICES, &verts[0].matrix[0][0], vertexCount * 16, DataChunkWriteStride(16, sizeof(Vertex)), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteStridedCounts(ChunkPropertyNames::VERTEX_SIZES, &verts[0].size, vertexCount, DataChunkWriteStride(1, sizeof(Vertex)), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::FREE_TEXT, "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        /*Path p("C:\\Users\\Derek\\Downloads\\test-combined.png");
        ByteBuffer imageBytes;
        p.ReadBinaryFile(imageBytes);
        meshFormatWriter.WriteBlob(ChunkPropertyNames::EMBEDDED_IMAGE, imageBytes.data(), imageBytes.size(), error);
        */
        uint8_t rawBytes[] = { TEST_BYTES };
        meshFormatWriter.WriteBlob(ChunkPropertyNames::EMBEDDED_IMAGE, rawBytes, sizeof(rawBytes), error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::EMBEDDED_QUOTE_TEXT, "->\"<- ->'<-", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::NEWLINE_TEXT, "someone\nwas\nhere", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::OPEN_CURLY, "{", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteString(ChunkPropertyNames::CLOSE_CURLY, "}", error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteChunk(ChunkNames::VERTICES, [](DataChunkWriter& vertsFormatWriter, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            double d[] =
            {
                1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5,
                11.5, 12.5, 13.5, 14.5, 15.5, 16.5, 17.5, 18.5, 19.5, 20.5,
                1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5,
                11.5, 12.5, 13.5, 14.5, 15.5, 16.5, 17.5, 18.5, 19.5, 20.5
            };
            vertsFormatWriter.WriteDoubles(ChunkPropertyNames::DOUBLES, d, FINJIN_COUNT_OF(d), error);
            if (error)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error);
                return;
            }
        }, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }

        meshFormatWriter.WriteChunk(ChunkNames::SUBMESHES, [](DataChunkWriter& submeshesWriter, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            for (size_t i = 0; i < 3; i++)
            {
                submeshesWriter.WriteChunk(i, [i](DataChunkWriter& submeshWriter, Error& error)
                {
                    FINJIN_ERROR_METHOD_START(error);

                    submeshWriter.WriteString(ChunkPropertyNames::NAME, Convert::ToString(i), error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }
                }, error);
                if (error)
                {
                    FINJIN_SET_ERROR_NO_MESSAGE(error);
                    return;
                }
            }
        }, error);
        if (error)
        {
            FINJIN_SET_ERROR_NO_MESSAGE(error);
            return;
        }
    }, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
}

void InMemoryReadTest(DataChunkReader& reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    std::ostream* debugOutput = nullptr; //&std::cout;

    std::cout << "Format = " << reader.GetHeader().format << std::endl;
    std::cout << "Version = " << reader.GetHeader().version << std::endl;
    std::cout << "Max bytes per line = " << reader.GetHeader().maxBytesPerLine << std::endl;

    using Callbacks = DataChunkReaderCallbacks<DataChunkReaderCallbacksState, 16>;
    Callbacks callbacks;
    {
        DataChunkReaderCallbacksChunkMapping<DataChunkReaderCallbacksState> defaultMapping;
        defaultMapping.propertyCallback = [debugOutput](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, DataChunkReaderCallbacksState& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto handled = true;

            if (propertyName == ChunkPropertyNames::CREATED)
            {
                DateTime value;
                reader.ReadDateTime(dataHeader, value, error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                    *debugOutput << propertyName.ToString() << " = " << value.ToIso8601() << std::endl;
            }
            else if (propertyName == ChunkPropertyNames::ID)
            {
                Uuid value;
                reader.ReadUuid(dataHeader, value, error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                    *debugOutput << propertyName.ToString() << " = " << value.ToString() << std::endl;
            }
            else
                handled = false;

            return handled;
        };
        callbacks.AddMapping(defaultMapping, error);
        if (error)
        {
            assert(0);
        }
    }
    {
        DataChunkReaderCallbacksChunkMapping<DataChunkReaderCallbacksState> mapping;
        mapping.pattern.AddName(ChunkNames::MESH);
        mapping.chunkCallback = [debugOutput](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, DataChunkReaderCallbacksState& state, Error& error)
        {
            if (debugOutput != nullptr)
                *debugOutput << "MESH " << (isStart ? "Start" : "End") << "-----------" << std::endl;
        };
        mapping.propertyCallback = [debugOutput](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, DataChunkReaderCallbacksState& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (propertyName == ChunkPropertyNames::NAME ||
                propertyName == ChunkPropertyNames::FREE_TEXT ||
                propertyName == ChunkPropertyNames::EMBEDDED_QUOTE_TEXT ||
                propertyName == ChunkPropertyNames::NEWLINE_TEXT ||
                propertyName == ChunkPropertyNames::OPEN_CURLY ||
                propertyName == ChunkPropertyNames::CLOSE_CURLY)
            {
                Utf8String value;
                reader.ReadString(dataHeader, value, error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                    *debugOutput << propertyName.ToString() << " = " << value << std::endl;
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_POSITIONS ||
                propertyName == ChunkPropertyNames::VERTEX_NORMALS)
            {
                auto readCount = reader.ReadStridedFloats(dataHeader, &verts[0].pos[0], vertexCount * 3, DataChunkReadStride(3, sizeof(Vertex)), error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    for (size_t i = 0; i < readCount; i++)
                        *debugOutput << propertyName.ToString() << " = " << verts[i / 3].pos[i % 3] << std::endl;
                }
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_TIMES)
            {
                auto readCount = reader.ReadStridedTimeDurations(dataHeader, &verts[0].t, vertexCount, DataChunkReadStride(1, sizeof(Vertex)), error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    for (size_t i = 0; i < readCount; i++)
                        *debugOutput << propertyName.ToString() << " = " << verts[i].t.ToString() << std::endl;
                }
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_BOOLS)
            {
                auto readCount = reader.ReadStridedBools(dataHeader, &verts[0].b, vertexCount, DataChunkReadStride(1, sizeof(Vertex)), error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    for (size_t i = 0; i < readCount; i++)
                        *debugOutput << propertyName.ToString() << " = " << (verts[i].b ? "true" : "false") << std::endl;
                }
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_STRINGS)
            {
                auto readCount = reader.ReadStridedStrings(dataHeader, &verts[0].s, vertexCount, DataChunkReadStride(1, sizeof(Vertex)), error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    for (size_t i = 0; i < readCount; i++)
                        *debugOutput << propertyName.ToString() << " = " << verts[i].s << std::endl;
                }
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_MATRICES)
            {
                auto readCount = reader.ReadStridedFloats(dataHeader, &verts[0].matrix[0][0], vertexCount * 16, DataChunkReadStride(16, sizeof(Vertex)), error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    float* matrixValues = &verts[0].matrix[0][0];
                    *debugOutput << propertyName.ToString() << " = ";
                    for (size_t i = 0; i < readCount; i++)
                    {
                        if (i > 0)
                            *debugOutput << ", ";
                        *debugOutput << GetStridedValue(matrixValues, i, DataChunkReadStride(16, sizeof(Vertex)));
                    }
                    *debugOutput << std::endl;
                }
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_SIZES)
            {
                auto readCount = reader.ReadStridedCounts(dataHeader, &verts[0].size, vertexCount, DataChunkReadStride(1, sizeof(Vertex)), error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    for (size_t i = 0; i < readCount; i++)
                        *debugOutput << propertyName.ToString() << " = " << verts[i].size << std::endl;
                }
            }
            else if (propertyName == ChunkPropertyNames::VERTEX_COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                    *debugOutput << propertyName.ToString() << " = " << value << std::endl;
            }
            else if (propertyName == ChunkPropertyNames::EMBEDDED_IMAGE)
            {
                uint8_t blobData[1000];
                auto blobDataSize = reader.ReadBlob(dataHeader, blobData, sizeof(blobData), error);
                if (error)
                    assert(0);
                auto blobBytes = static_cast<uint8_t*>(blobData);
                if (debugOutput != nullptr)
                {
                    *debugOutput << propertyName.ToString() << " = ";
                    for (size_t i = 0; i < blobDataSize; i++)
                    {
                        if (i > 0)
                            *debugOutput << ", ";
                        *debugOutput << (int)blobBytes[i];
                    }
                    *debugOutput << std::endl;
                }
            }
        };
        callbacks.AddMapping(mapping, error);
        if (error)
        {
            assert(0);
        }
    }
    {
        DataChunkReaderCallbacksChunkMapping<DataChunkReaderCallbacksState> mapping;
        mapping.pattern.AddName(ChunkNames::MESH);
        mapping.pattern.AddName(ChunkNames::VERTICES);
        mapping.chunkCallback = [debugOutput](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, DataChunkReaderCallbacksState& state, Error& error)
        {
            if (debugOutput != nullptr)
                *debugOutput << "VERTICES " << (isStart ? "Start" : "End") << "-----------" << std::endl;
        };
        mapping.propertyCallback = [debugOutput](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, DataChunkReaderCallbacksState& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (propertyName == ChunkPropertyNames::DOUBLES)
            {
                double value[100];
                auto readCount = reader.ReadDoubles(dataHeader, value, 100, error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                {
                    *debugOutput << propertyName.ToString() << " = ";
                    for (size_t readIndex = 0; readIndex < readCount; readIndex++)
                    {
                        if (readIndex > 0)
                            *debugOutput << ", ";
                        *debugOutput << value[readIndex];
                    }
                    *debugOutput << std::endl;
                }
            }
        };
        callbacks.AddMapping(mapping, error);
        if (error)
        {
            assert(0);
        }
    }
    {
        DataChunkReaderCallbacksChunkMapping<DataChunkReaderCallbacksState> mapping;
        mapping.pattern.AddName(ChunkNames::MESH);
        mapping.pattern.AddName(ChunkNames::SUBMESHES);
        mapping.chunkCallback = [debugOutput](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, DataChunkReaderCallbacksState& state, Error& error)
        {
            if (debugOutput != nullptr)
                *debugOutput << "SUBMESHES " << (isStart ? "Start" : "End") << "-----------" << std::endl;
        };
        callbacks.AddMapping(mapping, error);
        if (error)
        {
            assert(0);
        }
    }
    {
        DataChunkReaderCallbacksChunkMapping<DataChunkReaderCallbacksState> mapping;
        mapping.pattern.AddName(ChunkNames::MESH);
        mapping.pattern.AddName(ChunkNames::SUBMESHES);
        mapping.pattern.AddIndex();
        mapping.propertyCallback = [debugOutput](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, DataChunkReaderCallbacksState& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (propertyName == ChunkPropertyNames::NAME)
            {
                Utf8String value;
                reader.ReadString(dataHeader, value, error);
                if (error)
                {
                    assert(0);
                }
                if (debugOutput != nullptr)
                    *debugOutput << propertyName.ToString() << " = " << value << std::endl;
            }
        };
        callbacks.AddMapping(mapping, error);
        if (error)
        {
            assert(0);
        }
    }

    DataChunkReaderCallbacksState callbacksState;
    callbacks.ReadDocument(debugOutput, reader, callbacksState, error);
    if (error)
    {
        if (debugOutput != nullptr)
            *debugOutput << "Failed to read: " << error.ToString() << std::endl;
    }
}

template <typename WriterType, typename ReaderType>
bool DoReadWriteStreamingDocumentMemoryTest(size_t maxBytesPerLine, ByteOrder byteOrder)
{
    FINJIN_DECLARE_ERROR(error);

    WriterType::Settings writerSettings;
    writerSettings.maxBytesPerLine = maxBytesPerLine;
    writerSettings.byteOrder = byteOrder;

    ReaderType::Settings readerSettings;

    std::stringstream outStream;
    {
        StreamDataChunkWriterOutput chunkWriterOutput(outStream);
        writerSettings.Create(chunkWriterOutput, DefaultDataChunkWriterController::GetInstance());
        WriterType writer;
        writer.CreateRoot(writerSettings, error);
        BOOST_CHECK(!error);
        if (error)
        {
            std::cout << "Failed to create writer: " << error.ToString() << std::endl;
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
            return false;
        }
        else
        {
            InMemoryWriteTest(writer, error);
            BOOST_CHECK(!error);
            if (error)
            {
                std::cout << "Failed to write: " << error.ToString() << std::endl;
                BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
                return false;
            }
        }
    }

    if (!error)
    {
        auto outString = outStream.str();

        std::istringstream inStream(outString);
        StreamDataChunkReaderInput chunkReaderInput(inStream);
        readerSettings.Create(chunkReaderInput, DefaultDataChunkReaderController::GetInstance());
        ReaderType reader;
        reader.Create(readerSettings, nullptr, error);
        BOOST_CHECK(!error);
        if (error)
        {
            std::cout << "Failed to create reader: " << error.ToString() << std::endl;
            BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
            return false;
        }
        else
        {
            InMemoryReadTest(reader, error);
            BOOST_CHECK(!error);
            if (error)
            {
                std::cout << "Failed to read: " << error.ToString() << std::endl;
                BOOST_TEST_MESSAGE(error.GetLastNonEmptyErrorMessage());
                return false;
            }
        }
    }
    
    return true;
}

BOOST_AUTO_TEST_CASE(StreamingDataDocumentTest_in_memory)
{
    BOOST_TEST_MESSAGE("StreamingDataDocumentTest_in_memory:");

    RandomUuidCreator uuidCreator;

    size_t maxBytesPerLine = std::max((int)DataChunkReader::Settings::MIN_BYTES_PER_LINE, (int)DataChunkWriter::Settings::MIN_BYTES_PER_LINE);
    {
        auto bigBinaryStreamingTestResult = DoReadWriteStreamingDocumentMemoryTest<BinaryDataChunkWriter, BinaryDataChunkReader>(maxBytesPerLine, ByteOrder::BIG);
        BOOST_CHECK(bigBinaryStreamingTestResult == true);
    }
    {
        auto littleBinaryStreamingTestResult = DoReadWriteStreamingDocumentMemoryTest<BinaryDataChunkWriter, BinaryDataChunkReader>(maxBytesPerLine, ByteOrder::LITTLE);
        BOOST_CHECK(littleBinaryStreamingTestResult == true);
    }
    {
        //Byte order ignored for test
        auto textStreamingTestResult = DoReadWriteStreamingDocumentMemoryTest<TextDataChunkWriter, TextDataChunkReader>(maxBytesPerLine, ByteOrder::LITTLE);
        BOOST_CHECK(textStreamingTestResult == true);
    }
    {
        //Byte order ignored for test
        auto jsonStreamingTestResult = DoReadWriteStreamingDocumentMemoryTest<JsonDataChunkWriter, JsonDataChunkReader>(maxBytesPerLine, ByteOrder::LITTLE);
        BOOST_CHECK(jsonStreamingTestResult == true);
    } 
}
