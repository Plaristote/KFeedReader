const adPlaybackRate = 10;

function hasAds() {
  const adProgressBar = document.querySelector(".ad-interrupting .ytp-play-progress.ytp-swatch-background-color");
  return adProgressBar != null;
}

function hasSkipButton() {
  const button = document.querySelector(".ytp-skip-ad .ytp-skip-ad-button");
  return button != null;
}

function muteButton() {
  return document.querySelector(".ytp-mute-button.ytp-button");
}

class AdFixer {
  constructor() {
    this._mutedByScript = false;
    this.skipButtonNotified = false;
    setInterval(this.refreshStatus.bind(this), 1000);
  }

  get videos() {
    return Array.from(document.querySelectorAll("video"));
  }

  set adsPlaying(value) {
    if (value !== this.adsPlaying)
      value ? this.onAdsStarted() : this.onAdsStopped();
    this._adsPlaying = value;
  }

  get adsPlaying() {
    return this._adsPlaying === true;
  }

  set forceMute(value) {
    document.body.dataset.forceMute = value;
    if (value || (this._mutedByScript && !value)) {
      document.body.dataset.muteButton = value ? "clicked" : "unclicked";
      muteButton().click();
    }
    this._mutedByScript = value == true;
  }

  set playbackRate(value) {
    this.videos.forEach(function(video) { video.playbackRate = value; });
  }

  get playbackRate() {
    return this.videos[0].playbackRate;
  }

  get forceMute() {
    return this._mutedByScript === true;
  }

  get muted() {
    return this.videos.length == 0 || this.videos[0].muted;
  }

  toggleVisibility(visible) {
    const value = visible ? "" : "hidden";
    this.videos.forEach(video => video.style.visibility = value);
  }

  refreshStatus() {
    this.adsPlaying = hasAds();
    if (this.adsPlaying) this.onAdsUpdate();
  }

  onAdsUpdate() {
    this.playbackRate = adPlaybackRate;
    if (!this.skipButtonNotified && hasSkipButton()) {
      this.skipButtonNotified = true;
    }
  }

  onAdsStarted() {
    this.skipButtonNotified = false;
    this.toggleVisibility(false);
    this.backupPlaybackRate = this.playbackRate;
    this.playbackRate = adPlaybackRate;
    document.body.dataset.alreadyMuted = this.muted;
    if (!this.muted) this.forceMute = true;
    document.body.dataset.mutedAfterwards = this.muted;
  }

  onAdsStopped() {
    this.toggleVisibility(true);
    this.forceMute = false;
    this.playbackRate = this.backupPlaybackRate;
  }
}

if (window.location.host.match(/\.?youtube\.com$/))
  window.adFixer = new AdFixer();
