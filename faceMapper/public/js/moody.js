let facesArray

const onChangeImageUpload = (fileInput) => {
    if (fileInput.files.length) {
        createFaceGroups(fileInput.files)
        displayNodes()
    }
    fileInput.value = ''
}

const createFaceGroups = (files) => {
    const faceGroups = {}

    Array.from(files).forEach((file, index) => {
        const faceName = file.webkitRelativePath.split('/')[1]
        if (!faceGroups[faceName]) faceGroups[faceName] = []
        faceGroups[faceName] = [
            ...faceGroups[faceName],
            URL.createObjectURL(fileInput.files[index]),
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

const onClickExport = () => {
    if (!facesArray) return
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

const addMapping = (map, key, mapping) => {
    if (!map[key]) map[key] = []
    map[key].push(mapping.name.toUpperCase())
}

const generateMappings = () => {
    const mappings = {}

    faces.nodes.each((node) => {
        const iterator = node.findLinksOutOf()
        while (iterator.next()) {
            const item = iterator.value
            addMapping(mappings, item.data.from, facesArray[item.data.to])

            if (item.data.isBiDirectional) {
                addMapping(mappings, item.data.to, facesArray[item.data.from])
            }
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

const onClickSave = () => {
    const data = JSON.parse(faces.model.toJson())
    const nodeDataArray = data.nodeDataArray.map(({ source, ...item }) => item)
    data.nodeDataArray = nodeDataArray

    download('facesConfig.json', JSON.stringify(data))
}

const onChangeImport = (importInput) => {
    if (!facesArray) {
        window.alert('Please upload images first')
        importInput.value = ''
        return false
    }

    const files = importInput.files
    if (files.length <= 0) {
        importInput.value = ''
        return false
    }

    const fr = new FileReader()

    fr.onload = (e) => {
        const rawData = JSON.parse(e.target.result)
        const facesArrayObject = facesArray.reduce((previous, current) => {
            previous[current.name] = current.images
            return previous
        }, {})

        Object.entries(rawData.nodeDataArray).forEach((entry) => {
            const [key, node] = entry
            if (facesArrayObject[node.name]) {
                node.source = facesArrayObject[node.name][0]
            } else {
                delete rawData.nodeDataArray[key]
                rawData.nodeDataArray.length--
            }
        })

        faces.model = go.Model.fromJson(JSON.stringify(rawData))
        importInput.value = ''
    }

    fr.readAsText(files.item(0))
}
