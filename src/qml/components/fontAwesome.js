.pragma library

function fromText(str){
    return icons[str] || ""
}

var icons = {
    'search':       "\uf002",
    'fav':          "\uf004",
    'game':         "\uf11b",
    'chevron_r':    "\uf054",
    'chevron_l':    "\uf053",
    'spinner':      "\uf1ce",
}
