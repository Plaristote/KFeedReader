# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

add_executable(kfeedreader
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
    resources.qrc)

target_link_libraries(kfeedreader
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    Qt6::Multimedia
    Qt6::Xml
    Qt6::WebEngineQuick
    Qt6::Widgets
    KF6::I18n
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui)

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        list-add
        help-about
        application-exit
        applications-graphics
    )
endif()

kconfig_add_kcfg_files(kfeedreader GENERATE_MOC kfeedreaderconfig.kcfgc)
install(TARGETS kfeedreader ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
