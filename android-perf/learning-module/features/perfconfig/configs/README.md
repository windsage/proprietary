# 1. 创建平台特定目录结构
mkdir -p vendor/qcom/proprietary/android-perf/learning-module/features/perfconfig/configs/kalama
mkdir -p vendor/qcom/proprietary/android-perf/learning-module/features/perfconfig/configs/pineapple

# 2. 为每个平台创建配置文件（以kalama为例）
cp -f vendor/qcom/proprietary/android-perf/learning-module/features/perfconfig/configs/volcano/*  vendor/qcom/proprietary/android-perf/learning-module/features/perfconfig/configs/kalama/

# 3. 修改平台特定的Android.bp（以kalama为例）
sed -i 's/volcano/kalama/g' vendor/qcom/proprietary/android-perf/learning-module/features/perfconfig/configs/volcano/Android.bp

# 4. 修改`android-perf/configs/perf-product-vendor.mk`，增加特定平台
```
ifeq ($(call is-board-platform-in-list, kalama),true)
PRODUCT_PACKAGES += \
    perf_config.xml.kalama \
    PerfConfigFeature.xml.kalama
endif
```

# 5. 确认目录结构
tree vendor/qcom/proprietary/android-perf/learning-module/features/perfconfig/configs/

# 最终目录结构应该是：
# configs/
# ├── perf_config_validator.py
# ├── perfcore_encrypt.py
# ├── volcano/
# │   ├── Android.bp
# │   ├── perf_config.xml
# │   └── PerfConfigFeature.xml
# ├── kalama/
# │   ├── Android.bp
# │   ├── perf_config.xml
# │   └── PerfConfigFeature.xml
# └── pineapple/
#     ├── Android.bp
#     ├── perf_config.xml
#     └── PerfConfigFeature.xml
