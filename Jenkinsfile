pipeline {
    
    options {
        timestamps()
    }
    
    agent {
        node {
            label 'ubuntu-build'
            }   // end of node
    }    // end of agent     
    
    stages {
            stage ('Checkout') {
                steps {
                    checkout scm
                    //git 'https://github.com/yackey/AEF-CI.git'
                    stash name: 'everything', 
                        excludes: 'test-results/**', 
                        includes: '**'
                }   // end of steps
            }   // end of checkout stage  

            stage ('Parallel ARM Build') {
                failFast true
                parallel {

                    stage ('Release ARM Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            dir ("build") {
                                Build_ARM_Release()
                            }   // end of dir
                        }   // end of steps
                    }   // end of Release ARM Build stage

                    stage ('Debug ARM Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            dir ("build") {
                                Build_ARM_Debug()
                            }   // end of dir
                        }   // end of steps
                    }   // end of Debug ARM Build stage

                }   //end of parallel ARM
            }

            stage ('Parallel Simulator Build') {
                failFast true
                parallel {

                    stage ('Release Simulator Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            dir ("build") {
                                Build_x86_64_Release()
                            }   // end of dir
                        }   // end of steps
                    }   // end of Release Simulator Build stage

                    stage ('Debug Simulator Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            dir ("build") {
                                Build_x86_64_Debug()
                            }   // end of dir
                        }   // end of steps
                    }   // end of Debug Simulator Build stage
                    
                }   //end of parallel simulator
            }
    }   // end of stages        
}

def Build_x86_64_Release() {
    sh 'rm -rf *'
    unstash 'everything'
    sh 'conan install ../test -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-x86_64.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TARGET_PROCESSOR=x86_64 ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release -f'
}

def Build_x86_64_Debug() {
    sh 'rm -rf *'
    unstash 'everything'
    sh 'conan install ../test -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-x86_64.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TARGET_PROCESSOR=x86_64 ..'
    sh 'cmake --build .'
    //
    sh './x86_64/bin/Debug/AefTest --gtest_output=xml:AefTestResultsDebug.xml'
    step([$class: 'JUnitResultArchiver', allowEmptyResults: true, testResults: 'AefTestResultsDebug.xml'])
    //  
    sh '/usr/bin/lcov --directory . --capture --output-file ./code_coverage.info -rc lcov_branch_coverage=1' 
    sh '/usr/bin/genhtml code_coverage.info --branch-coverage --output-directory ./code_coverage_report_Debug'                        
    //
    publishHTML([allowMissing: false, 
        alwaysLinkToLastBuild: false, 
        keepAll: true, 
        reportDir: 'code_coverage_report_Debug', 
        reportFiles: 'index.html', 
        reportName: 'Code Coverage Debug', 
        reportTitles: ''])                         
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug -f'
}

def Build_ARM_Release() {
    sh 'rm -rf *'
    unstash 'everything'
    sh 'conan install ../test -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-ARM.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TARGET_PROCESSOR=ARM ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release -f'
}

def Build_ARM_Debug() {
    sh 'rm -rf *'
    unstash 'everything'
    sh 'conan install ../test -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-ARM.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TARGET_PROCESSOR=ARM ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug -f'
}