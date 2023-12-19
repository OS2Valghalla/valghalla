# Old version!
This version of OS2valghalla based on Drupal 7 has been archived. Please find the new OS2valghalla repository at https://github.com/OS2Valghalla 

Valghalla - rekruttering af valgtilforordnede
-----------------------------
Valghalla er et webbaseret system til håndtering af valgstyrere og tilforordnede i forbindelse med valg. Løsningen er oprindeligt udviklet af Aarhus Kommune og gratis tilgængeligt for alle.

Valghalla er udviklet til at forenkle og digitalisere processen med at rekruttere tilforordnede til valg. Og så er den Open Source, så andre kommuner kan få glæde af den. Valghalla er et produkt der udvikles og vdeligeholdes af OS2. Se mere her: https://os2.eu/os2valghalla

De overordnede mål er:

* Digitalisere en tung, manuel opgave og bibeholde meget højt datasikkerhedsniveau
* Anvendelse af automatiserede e-mails, der hvor det er muligt
* Give valgsekretariatet i kommunen et simpelt overblik over en kompleks opgave
* Give partiforeningerne overblikket over deres andel af valget
* Mest mulig uddelegering ved at smidiggøre partiforeningernes opgave
* Størst mulig automatiseringsgrad af bekræftelser m.m.
* Hurtigere effektuering ved efterfølgende valg gennem genbrug af data

## Build status

Develop branch state ![alt text](https://travis-ci.org/bellcom/valghalla.svg?branch=develop)

## Deployment new changes

To deploy new changes and apply all deploy actions please use following steps. 

* Check if local code changes are present (uncommitted changes) `git diff`
  * If yes, review them and eventually reset `git checkout [path to changed file]`
* Fetch new tags `git fetch origin 'refs/tags/*:refs/tags/*'`
* Checkout to tag you deploying `git checkout [tag name]`
* Switch to drupal root directory `cd public_html` 
* Run deployment script `sh ../scripts/deploy-multisite.sh`

Deployment output log will be saved to `logs/deployment/deployment-[date]-[time].log` file. 

## Git branches name convention in versions perspective.
| Syntax      | Status               | Description 
| ----        | :----                | :----        
| **develop** | [ACTIVE DEVELOPMENT] | Reflects latest changes that have been merged into master or should be merged soon.
| **7.x-2.x** | [EOL]                | Version 2.x. EOL 2018. Support fixes. Drupal core update.
| **7.x-3.x** | [EOL]                | Version 3.x. EOL June 2019. Support fixes. Drupal core update.
| **7.x-4.x** | [CURRENT]            | Version 4.x. All new feature should be merge here.
| **7.x-5.x** | [UPCOMING]           | Version 5.x. No branch created.
