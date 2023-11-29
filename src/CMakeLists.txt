# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

add_executable(kfeedreader
    main.cpp
    app.cpp
    menuitemmodel.cpp
    menuitem.cpp
    feedfolder.cpp
    feed.cpp
    feedupdate.cpp
    feedarticle.cpp
    feedarticlemedia.cpp
    feedarticleenclosure.cpp
    aggregatedfeed.cpp
    reader-rss.cpp
    reader-atom.cpp
    reader-json.cpp
    resources.qrc)

target_link_libraries(kfeedreader
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Svg
    Qt5::Multimedia
    Qt5::Xml
    Qt5::WebEngine
    KF5::I18n
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::ConfigGui)

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
