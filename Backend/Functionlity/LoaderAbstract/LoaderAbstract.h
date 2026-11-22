#pragma once

#include <iostream>
#include <cstring>
#include <clang-c/Index.h>
#include <cassert>
#include <filesystem>
#include <vector>

#include "StoreJson.h"

struct LoaderAbstract {

private:
    std::string SaveFile = "./Sys/Base.json";

public:

    static CXChildVisitResult astVisitor(CXCursor cursor, CXCursor parent, CXClientData clientData) {
        LoaderAbstract* self = static_cast<LoaderAbstract*>(clientData);

        if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl) {
            self->FunctionVisitor(cursor);
        }

        return CXChildVisit_Recurse;
    }

    void parseHeaderFiles(std::vector<std::string>& FileNames, std::string SFile, bool Erase) {
        if (FileNames.empty()) return;
        if (SFile.empty()) return;

        if (Erase) FileClean(SFile);

        SaveFile = SFile;
        CXIndex index = clang_createIndex(0, 0);

        for (std::string& File : FileNames) {
            if (!std::filesystem::exists(File)) continue;
            CXTranslationUnit translationUnit = clang_parseTranslationUnit(
                index,
                File.c_str(),
                nullptr, 0,
                nullptr, 0,
                CXTranslationUnit_None
            );

            assert(translationUnit && "Failed To Parse Header");

            CXCursor rootCursor = clang_getTranslationUnitCursor(translationUnit);
            clang_visitChildren(rootCursor, astVisitor, this);

            clang_disposeTranslationUnit(translationUnit);
        }

        FileRemoveComma(SaveFile);
        clang_disposeIndex(index);

        return;
    }


private:
    void FunctionVisitor(CXCursor cursor) {
        FunctionData FD{};

        //get function name
        CXString name = clang_getCursorSpelling(cursor);

        //get function return type
        CXType cursorType = clang_getCursorType(cursor);
        CXType type = clang_getResultType(cursorType);
        CXString typeStr = clang_getTypeSpelling(type);

        //description
        CXString RawDescription = clang_Cursor_getRawCommentText(cursor);
        const char* DesStr = clang_getCString(RawDescription);

        //settings
        FD.Name = clang_getCString(name);
        FD.Type = clang_getCString(typeStr);
        if (DesStr) FD.Description = DesStr;

        //accessibility
        CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);
        switch (access) {
        case CX_CXXPublic:
            FD.Access = "public";
            break;
        case CX_CXXPrivate:
            FD.Access = "private";
            break;
        default:
            FD.Access = "public";
            break;
        }

        clang_disposeString(name);
        clang_disposeString(typeStr);
        clang_disposeString(RawDescription);

        //setting and getting of parameters
        int args_len = clang_Cursor_getNumArguments(cursor);
        FD.Parameter.resize(args_len);
        for (int i = 0; i < args_len; i++) {
            CXCursor argCursor = clang_Cursor_getArgument(cursor, i);

            //nameing
            CXString argName = clang_getCursorDisplayName(argCursor);
            FD.Parameter[i][1] = clang_getCString(argName);
            clang_disposeString(argName);

            //setting type
            CXType argType = clang_getCursorType(argCursor);

            bool isRef = (argType.kind == CXType_LValueReference || argType.kind == CXType_RValueReference);

            CXType baseType = argType;
            if (isRef || argType.kind == CXType_Pointer) baseType = clang_getPointeeType(baseType);

            if (clang_isConstQualifiedType(argType)) FD.Parameter[i][2] = "true";
            else FD.Parameter[i][2] = "false";

            CXString argTypeStr = clang_getTypeSpelling(baseType);
            FD.Parameter[i][0] = clang_getCString(argTypeStr);
            clang_disposeString(argTypeStr);
        }

        FunctionJson(FD, this->SaveFile);
    }
};
