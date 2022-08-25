let facesArray

const onChangeFileInput = (fileInput) => {
    if (fileInput.files.length) {
        createFaceGroups(fileInput.files)
        displayNodes()
    }
}

const createFaceGroups = (files) => {
    const faceGroups = {}

    Array.from(files).forEach((file) => {
        const faceName = file.webkitRelativePath.split('/')[1]
        if (!faceGroups[faceName]) faceGroups[faceName] = []
        faceGroups[faceName] = [
            ...faceGroups[faceName],
            `../${file.webkitRelativePath}`,
        ]
    })

    facesArray = []
    Object.keys(faceGroups).forEach((face) => {
        facesArray.push({ name: face, images: faceGroups[face] })
    })

    facesArray.sort((a, b) => a.name.localeCompare(b.name))
}

const displayNodes = () => {
    const faceNodes = facesArray.map((face, index) => {
        return {
            key: index,
            source: face.images[0],
            text: `${index} - ${face.name}`,
            name: face.name,
        }
    })

    faces.model = new go.GraphLinksModel(faceNodes, [])
}

const onClickSave = () => {
    download('facesConfig.h', generateConfigFile())
}

const generateConfigFile = () => {
    let mappings = generateMappings()
    const maxLinks = getMaxLinks(mappings)
    const fileMappings = generateFileMappings(mappings, maxLinks)

    return `#define NUMBER_FACES ${facesArray.length}
#define INVALID_FACE -1
${generateDefines()}
const int8_t nextFaces[][${maxLinks}] = {
${fileMappings}};\n`
}

const generateDefines = () => {
    let defines = ''
    facesArray.forEach((face, index) => {
        defines += `#define ${face.name.toUpperCase()} ${index}\n`
    })
    return defines
}

const getMaxLinks = (mappings) => {
    let max = 0
    Object.keys(mappings).forEach((mapping) => {
        max = Math.max(max, mappings[mapping].length)
    })
    return max
}

const generateMappings = () => {
    const mappings = {}

    faces.nodes.each((node) => {
        const iterator = node.findLinksOutOf()
        while (iterator.next()) {
            const item = iterator.value
            if (!mappings[node.qb.key]) mappings[node.qb.key] = []

            mappings[node.qb.key].push(
                facesArray[item.qb.to].name.toUpperCase()
            )
        }
    })

    return mappings
}

const generateFileMappings = (mappings, max) => {
    Object.keys(mappings).forEach((mapping) => {
        while (mappings[mapping].length < max) {
            mappings[mapping].push('INVALID_FACE')
        }
    })

    let fileMappings = ''

    facesArray.forEach((face, index) => {
        if (!mappings[index]) mappings[index] = Array(max).fill('INVALID_FACE')
        fileMappings += `  {${mappings[index].join(', ')}}`
        if (index !== facesArray.length - 1) fileMappings += ','
        fileMappings += ` //${face.name.toUpperCase()}\n`
    })
    return fileMappings
}

const download = (filename, text) => {
    const element = document.createElement('a')
    element.setAttribute(
        'href',
        `data:text/plain;charset=utf-8,${encodeURIComponent(text)}`
    )
    element.setAttribute('download', filename)

    element.style.display = 'none'
    document.body.appendChild(element)

    element.click()

    document.body.removeChild(element)
}

const onClickExport = () => {
    download('facesConfig.json', faces.model.toJson())
}

const onChangeImport = (importInput) => {
    const files = importInput.files
    if (files.length <= 0) {
        return false
    }

    const fr = new FileReader()

    fr.onload = (e) => {
        faces.model = go.Model.fromJson(e.target.result)
    }

    fr.readAsText(files.item(0))
}
