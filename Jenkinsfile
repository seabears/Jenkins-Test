pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps {
                git branch: 'main', url: 'https://github.com/seabears/Jenkins-Test.git'
            }
        }

        stage('Lint') {
            steps {
                echo '변경된 파일 목록 추출 중...'

                script {
                    // 추가, 변경, 복사된 C, H 파일 모두 추출
                    def changedFiles = sh(
                        script: "git diff --name-only --diff-filter=ACM origin/main | grep -E '\\.(c|h)$' || true",
                        returnStdout: true
                    ).trim()

                    if (changedFiles == "") {
                        echo "변경된 C 소스 파일이 없습니다."
                        currentBuild.result = 'SUCCESS'
                        return
                    }

                    echo "변경된 파일:\n${changedFiles}"

                    // cppcheck 실행 시 파일명 여러개 공백으로 구분하여 넘기기
                    sh "cppcheck --enable=all --inconclusive --quiet --force ${changedFiles.replaceAll('\\n', ' ')}"
                }
            }
        }

        stage('Build') {
            steps {
                echo '빌드 단계 진행 중...'
                sh 'gcc -o test1 test1.c'
            }
        }

        stage('Test') {
            steps {
                echo '테스트 단계 진행 중...'
            }
        }
    }
}
