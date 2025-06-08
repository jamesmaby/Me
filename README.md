git init
git remote add origin https://github.com/ton-utilisateur/ton-repo.git
git config --global user.email "enter@mail.com"
git config --global user.name "james"
git add .
git commit -m "Initial commit"
git push -u origin main

Si conflits : 
git pull --no-rebase --allow-unrelated-histories origin main
