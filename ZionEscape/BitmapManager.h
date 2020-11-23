#pragma once

#ifndef _BITMAPMANAGER_H_
#define _BITMAPMANAGER_H_

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

/**
 * @brief BitmapManager is a class used to work with Bitmaps and load
 *        them into memory, only when needed. Allows Bitmap preloading
 *        and caching.
*/
ref class BitmapManager {
  static BitmapManager^ instance;
  Dictionary<String^, Bitmap^>^ images;

private:
  BitmapManager() {
    images = gcnew Dictionary<String^, Bitmap^>();
  }

  ~BitmapManager() {
    for each (KeyValuePair<String^, Bitmap^> img in images) {
      if (img.Value != nullptr)
        delete img.Value;
      delete img.Key;
    }
    images->Clear();
    delete images;
  }

public:
  /**
   * @brief Gets the global instance of BitmapManager. Creates one if it doesn't exist.
   * @return BitmapManager global instance.
  */
  static BitmapManager^ GetInstance() {
    if (instance == nullptr)
      instance = gcnew BitmapManager();

    return instance;
  }

  /**
   * @brief Checks if the BitmapManager has already loaded an image.
   * @param path URI to the file path, relative to the executable file or absolute.
   * @return True if the URI was found, false otherwise.
  */
  bool HasImage(String^ path) {
    return images->ContainsKey(path);
  }

  /**
   * @brief Checks if the BitmapManager has already loaded an image.
   * @param bmp Pointer to another Bitmap.
   * @return True if the Bitmap was found, false otherwise.
  */
  bool HasImage(Bitmap^ bmp) {
    return images->ContainsValue(bmp);
  }

  /**
   * @brief Allows the preloading of a Bitmap into memory.
   * @param path URI to the file path, relative to the executable file or absolute.
  */
  void PreloadImage(String^ path) {
    if (!images->ContainsKey(path)) {
      Bitmap^ bmp = LoadImage(path);
      images->Add(path, bmp);
    }
  }

  /**
   * @brief Delete from memory an specific Bitmap.
   * @param path URI to the file path, relative to the executable file or absolute.
  */
  void ReleaseImage(String^ path) {
    Bitmap^ bmp;
    if (images->TryGetValue(path, bmp)) {
      images->Remove(path);
      delete bmp;
    }
  }

  /**
   * @brief Get a Bitmap from the filesystem if it is not already loaded in the memory.
   * @param path URI to the file path, relative to the executable file or absolute.
   * @return Pointer to the Bitmap of the URI requested.
  */
  Bitmap^ GetImage(String^ path) {
    Bitmap^ bmp;
    if (!images->TryGetValue(path, bmp)) {
      bmp = LoadImage(path);
      images->Add(path, bmp);
    }
    return bmp;
  }

  /**
   * @brief Get a Bitmap from the filesystem if it is not already loaded in the memory.
   * @param path URI to the file path, relative to the executable file or absolute.
   * @return Pointer to the Bitmap of the URI requested.
  */
  Bitmap^ GetImage(Uri^ uri) {
    String^ path = uri->ToString();
    Bitmap^ bmp;
    if (!images->TryGetValue(path, bmp)) {
      bmp = LoadImage(path);
      images->Add(path, bmp);
    }
    return bmp;
  }

private:
  /**
   * @brief Utility function to create a Bitmap pointer from an URI.
   * @param path URI to the file path, relative to the executable file or absolute.
   * @return Pointer to the Bitmap of the URI requested.
  */
  Bitmap^ LoadImage(String^ path) {
    Bitmap^ bmp = gcnew Bitmap(path);
    return bmp;
  }
};

#endif // !_BITMAPMANAGER_H_

