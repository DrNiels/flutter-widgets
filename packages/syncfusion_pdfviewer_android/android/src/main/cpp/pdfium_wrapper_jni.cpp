#include <jni.h>
#include <fpdfview.h>
#include <new>
#include <stdlib.h>

extern "C" {

/**
 * Structure to hold PDFium document handle and its backing buffer.
 * We keep the buffer pointer so we can free it when closing the document.
 */
struct PdfiumDocument {
    FPDF_DOCUMENT doc; 
    uint8_t* buffer;
};

/**
 * Helper to safely cast jlong handle to PdfiumDocument* and validate it.
 * Returns nullptr if invalid.
 */
static inline PdfiumDocument* asDocument(jlong handle) {
    if (handle == 0) return nullptr;
    PdfiumDocument* document = reinterpret_cast<PdfiumDocument*>(handle);
    if (!document || !document->doc) return nullptr;
    return document;
}

JNIEXPORT jboolean JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_initLibrary(JNIEnv* env, jobject thiz) {
    FPDF_InitLibraryWithConfig(nullptr);
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_destroyLibrary(JNIEnv* env, jobject thiz) {
    FPDF_DestroyLibrary();
}

JNIEXPORT jlong JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_loadDocument(JNIEnv* env, jobject thiz, jbyteArray pdfData, jstring password) {
    if (pdfData == nullptr) {
        return 0;
    }

    jsize length = env->GetArrayLength(pdfData);
    if (length <= 0) return 0;
    // Allocate buffer and copy PDF data
    uint8_t* buffer = static_cast<uint8_t*>(malloc(static_cast<size_t>(length)));
    if (!buffer) return 0;

    env->GetByteArrayRegion(pdfData, 0, length, reinterpret_cast<jbyte*>(buffer));
    const char* pwd = password ? env->GetStringUTFChars(password, nullptr) : nullptr;
    FPDF_DOCUMENT doc = FPDF_LoadMemDocument64(buffer, static_cast<size_t>(length), pwd);
    if (pwd) {
        env->ReleaseStringUTFChars(password, pwd);
    }
    if (!doc) {
        free(buffer);
        return 0;
    }

     // Create the wrapper object that owns both document and buffer
    PdfiumDocument* document = new (std::nothrow) PdfiumDocument();
    if (!document) {
        FPDF_CloseDocument(doc);
        free(buffer);
        return 0;
    }

    document->doc = doc;
    document->buffer = buffer;
    return reinterpret_cast<jlong>(document);
}

JNIEXPORT jlong JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_loadDocumentFromFile(JNIEnv* env, jobject thiz, jstring path, jstring password) {
    if (path == nullptr) {
        return 0;
    }

    const char* filePath = env->GetStringUTFChars(path, nullptr);
    const char* pwd = password ? env->GetStringUTFChars(password, nullptr) : nullptr;

    FPDF_DOCUMENT doc = FPDF_LoadDocument(filePath, pwd);
    env->ReleaseStringUTFChars(path, filePath);
    if (pwd) {
        env->ReleaseStringUTFChars(password, pwd);
    }

    if (!doc) {
        return 0;
    }

    // Create the wrapper object
    PdfiumDocument* document = new (std::nothrow) PdfiumDocument();
    if (!document) {
        FPDF_CloseDocument(doc);
        return 0;
    }

    document->doc = doc;
    document->buffer = nullptr;  // No buffer to free
    return reinterpret_cast<jlong>(document);
}

JNIEXPORT void JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_closeDocument(JNIEnv* env, jobject thiz, jlong docHandle) {
    if (docHandle == 0) return;
    PdfiumDocument* document = reinterpret_cast<PdfiumDocument*>(docHandle);
    if (!document) return;
    if (document->doc != nullptr) {
        FPDF_CloseDocument(document->doc);
    }
    if (document->buffer != nullptr) {
        free(document->buffer);
    }
    delete document;
}

JNIEXPORT jint JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_getPageCount(JNIEnv* env, jobject thiz, jlong docHandle) {
    PdfiumDocument* document = asDocument(docHandle);
    return document ? FPDF_GetPageCount(document->doc) : -1;
}

JNIEXPORT jfloat JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_getPageWidth(JNIEnv* env, jobject thiz, jlong docHandle, jint index) {
    PdfiumDocument* document = asDocument(docHandle);
    if (!document) return -1;

    int count = FPDF_GetPageCount(document->doc);
    if (index < 0 || index >= count) return -1;

    FPDF_PAGE page = FPDF_LoadPage(document->doc, index);
    if (!page) return -1;
    jfloat width = FPDF_GetPageWidthF(page);
    FPDF_ClosePage(page);
    return width;
}

JNIEXPORT jfloat JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_getPageHeight(JNIEnv*, jobject, jlong docHandle, jint index) {
    PdfiumDocument* document = asDocument(docHandle);
    if (!document) return -1;

    int count = FPDF_GetPageCount(document->doc);
    if (index < 0 || index >= count) return -1;

    FPDF_PAGE page = FPDF_LoadPage(document->doc, index);
    if (!page) return -1;

    jfloat height = FPDF_GetPageHeightF(page);
    FPDF_ClosePage(page);
    return height;
}

JNIEXPORT jbyteArray JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_renderPage(JNIEnv* env, jobject thiz, jlong docHandle, jint pageIndex, jint width, jint height) {
    PdfiumDocument* document = asDocument(docHandle);
    if (!document) return nullptr;

    int pageCount = FPDF_GetPageCount(document->doc);
    if (pageIndex < 0 || pageIndex >= pageCount) return nullptr;

    FPDF_PAGE page = FPDF_LoadPage(document->doc, pageIndex);
    if (!page) return nullptr;

    FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
    if (!bitmap) {
        FPDF_ClosePage(page);
        return nullptr;
    }

    FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);
    FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, FPDF_LCD_TEXT | FPDF_REVERSE_BYTE_ORDER);

    void* buffer = FPDFBitmap_GetBuffer(bitmap);
    int stride = FPDFBitmap_GetStride(bitmap);
    int totalSize = stride * height;

    jbyteArray result = env->NewByteArray(totalSize);
    if (result != nullptr) {
         env->SetByteArrayRegion(result, 0, totalSize, reinterpret_cast<jbyte*>(buffer));
    }

    FPDFBitmap_Destroy(bitmap);
    FPDF_ClosePage(page);

    return result;
}

JNIEXPORT jbyteArray JNICALL
Java_com_syncfusion_flutter_pdfviewer_android_PdfiumAdapter_renderTile(JNIEnv* env, jobject thiz,
                                                 jlong docHandle,
                                                 jint pageIndex,
                                                 jfloat x, jfloat y,
                                                 jint width, jint height,
                                                 jfloat scale) {
    PdfiumDocument* document = asDocument(docHandle);
    if (!document) return nullptr;

    int pageCount = FPDF_GetPageCount(document->doc);
    if (pageIndex < 0 || pageIndex >= pageCount) return nullptr;

    FPDF_PAGE page = FPDF_LoadPage(document->doc, pageIndex);
    if (!page) return nullptr;

    FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 0);
    if (!bitmap) {
        FPDF_ClosePage(page);
        return nullptr;
    }

    FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);

    FS_MATRIX matrix = {
        scale, 0, 0,
        scale, -x * scale, -y * scale
    };

    FS_RECTF clip = {0, 0, static_cast<float>(width), static_cast<float>(height) };

    FPDF_RenderPageBitmapWithMatrix(bitmap, page, &matrix, &clip, FPDF_LCD_TEXT | FPDF_REVERSE_BYTE_ORDER);

    void* buffer = FPDFBitmap_GetBuffer(bitmap);
    int stride = FPDFBitmap_GetStride(bitmap);
    int totalSize = stride * height;

    jbyteArray result = env->NewByteArray(totalSize);
    if (result != nullptr) {
        env->SetByteArrayRegion(result, 0, totalSize, reinterpret_cast<jbyte*>(buffer));
    }

    FPDFBitmap_Destroy(bitmap);
    FPDF_ClosePage(page);

    return result;
}

} // extern "C"
