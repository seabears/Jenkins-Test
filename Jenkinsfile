pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps {
                // 명시적으로 main 브랜치에서 소스 체크아웃
                git branch: 'main', url: 'https://github.com/seabears/Jenkins-Test.git'
            }
        }

        stage('Build') {
            steps {
                echo '빌드 단계 진행 중...'
                // 예: gcc 컴파일 커맨드 (실제 빌드 스크립트 넣기)
                sh 'gcc -o test1 test1.c'
            }
        }

        stage('Test') {
            steps {
                echo '테스트 단계 진행 중...'
                // 테스트 커맨드 삽입 가능
            }
        }
    }
}
