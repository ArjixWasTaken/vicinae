#pragma once
#include "favicon-request.hpp"
#include "image-fetcher.hpp"

class GoogleFaviconRequester : public AbstractFaviconRequest {
  const std::vector<uint> sizes = {128, 64, 32, 16};
  size_t currentSizeAttemptIndex = 0;
  QString placeholderUrl;
  ImageReply *_currentReply;

  QString makeUrl(uint size) const;
  void loadingFailed();
  void imageLoaded(QPixmap pixmap);
  void tryForCurrentSize();

public:
  void start() override;

  GoogleFaviconRequester(const QString &domain, QObject *parent = nullptr);
  ~GoogleFaviconRequester();
};
