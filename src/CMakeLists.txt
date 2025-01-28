set(kfeedreader_src
    opml/import.cpp
    opml/export.cpp
    youtube/sync.cpp
    youtube/favicon.cpp
    main.cpp
    app.cpp
    menuitem.cpp
    feedfolder.cpp
    feed.cpp
    feedfetch.cpp
    feedfetch_customrequestcreators.cpp
    feedupdate.cpp
    feedfavicon.cpp
    feedarticle.cpp
    feedarticlemedia.cpp
    feedarticleenclosure.cpp
    feednavigation.cpp
    aggregatedfeed.cpp
    reader-rss.cpp
    reader-rdf.cpp
    reader-atom.cpp
    reader-json.cpp
    ttlsettings.cpp
    htmlprobe.cpp
    sharingservice.cpp
    javascriptplugins.cpp
    resources.qrc)

set(kfeedreader_link_libraries
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    Qt6::Multimedia
    Qt6::Xml
    Qt6::${WEBVIEW_PROVIDER}
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui)

if ("${WEBVIEW_PROVIDER}" STREQUAL "WebEngineQuick")
  message(STATUS "Using webengine.qrc")
  set(kfeedreader_src ${kfeedreader_src} webengine.qrc)
else()
  message(STATUS "Using webview.qrc")
  set(kfeedreader_src ${kfeedreader_src} webview.qrc)
endif()

if (WIN32)
    add_executable(fluxkap ${kfeedreader_src} contents/windows.rc)
else()
    add_executable(fluxkap ${kfeedreader_src})
endif()

if (NOT ANDROID)
  set(kfeedreader_link_libraries
    ${kfeedreader_link_libraries}
    Qt6::Widgets)
endif()

target_link_libraries(fluxkap ${kfeedreader_link_libraries})

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        list-add
        help-about
        application-exit
        applications-graphics
    )
endif()

kconfig_add_kcfg_files(fluxkap GENERATE_MOC fluxkapconfig.kcfgc)
install(TARGETS fluxkap ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
