add_library(engine_dict_static STATIC dictengine.cpp)
target_add_definitions(engine_dict_static -DTRANSLATION_DOMAIN="plasma_addons_engine_dict")

target_link_libraries (engine_dict_static
  KF5::I18n
  Qt::Network
)

