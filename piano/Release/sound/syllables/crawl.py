import requests
session=requests.Session()
for i in range(0,7):
    for j in ['C','D','E','F','G','A','B']:
        url='https://cdn.jsdelivr.net/gh/warpprism/cdn@latest/autopiano/static/samples/bright_piano/'+j+str(i).lower()+'.mp3'
        with open(j+str(i)+'.mp3','wb') as fd:
            fd.write(session.get(url).content)
        
