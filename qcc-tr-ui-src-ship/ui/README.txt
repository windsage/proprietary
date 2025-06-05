Now QCC-TR-UI APK will be delivered as prebuilt apk.

If any changes will be required in QCC-TR-UI.apk then follow these steps for enabling JAVA file compilation in vendor which is required for generating QCC-TR-UI APK:

Comment below mentioned lines in Android.bp file:
//java_import {
//	name : "qccauthmgr_jar",
//	jars : ["libs/qccauthmgrlib.jar"],
//}
//android_app {
//
//    name: "QCC-TR-UI",
//    owner: "qti",
//    proprietary: true,
//
//	static_libs: [
//		"qccauthmgr_jar",
//		"android-support-v7-appcompat",
//	],
//
//    srcs: [
//        "./**/*.java",
//        "./**/I*.aidl",
//    ],
//    optimize: {
//        enabled: false,
//    },
//	certificate: "platform",
//    sdk_version: "system_current",
//}

below lines in build.gradle file for build in androidStudio env
buildscript {
    repositories {
        mavenCentral()
        jcenter()
        google()
    }

    dependencies {
	classpath 'com.android.tools.build:gradle:4.1.3'
    }
}


apply plugin: 'com.android.application'

def renameArtifact(variant) {
    variant.outputs.all { output ->
        def fullName = output.outputFile.name
        def projectName = fullName.substring(0, fullName.indexOf('-'))
        outputFileName = projectName+"QCC-TR-UI"
    }
}

android {
    compileSdkVersion 33

    defaultConfig {
        applicationId "com.qualcomm.qti.smq"
        minSdkVersion 28
        targetSdkVersion 33
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
            applicationVariants.all { variant ->
                variant.outputs.all {
                    outputFileName = "QCC-TR-UI_${variant.buildType.name}.apk"
                }
            }
        }
        debug {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
            applicationVariants.all { variant ->
                variant.outputs.all {
                    outputFileName = "QCC-TR-UI_${variant.buildType.name}.apk"
                }
            }
        }
    }

    sourceSets {
        main {
            manifest.srcFile 'AndroidManifest.xml'
            java.srcDir 'src'
            res.srcDir 'res'
            //res.srcDirs = ['res']
            assets.srcDir 'assets'
            jniLibs.srcDirs = ['libs']
        }
    }

    lintOptions {
        abortOnError false
        disable 'MissingTranslation'
    }
    compileOptions {
        sourceCompatibility JavaVersion.VERSION_1_8
        targetCompatibility JavaVersion.VERSION_1_8
    }

    buildTypes {
        debug {
            debuggable true
        }
    }
}

def getLayoutLibPath() { return "${android.getSdkDirectory().getAbsolutePath()}" + "/platforms/"+android.compileSdkVersion + "/data/layoutlib.jar" }


allprojects {
    repositories {
        mavenCentral()
        jcenter()
        google()
    }
}

repositories {
    flatDir {
        dirs 'libs'
    }
}


dependencies{
    implementation 'androidx.appcompat:appcompat:1.3.1'
    implementation 'androidx.annotation:annotation:1.2.0'
    implementation files('libs/qccauthmgrlib.jar')
}