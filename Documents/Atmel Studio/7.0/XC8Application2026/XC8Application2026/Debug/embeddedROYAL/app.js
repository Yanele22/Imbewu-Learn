const app=document.querySelector('#app');
const pageTitle=document.querySelector('#pageTitle');
const pageMeta=document.querySelector('#pageMeta');
const topbarRight=document.querySelector('#topbarRight');
const roleButtons=document.querySelectorAll('#roleSwitch button');
const navTabs=document.querySelector('#navTabs');

const routes={
  '/':home,
  '/documents':documents,
  '/help':help,
  '/adviser':adviser,
  '/area/family':()=>area('My Family','Life cover, funeral plan and the people you protect.',[
    ['Life cover','Arranged by Royal Square','R1 500 000 cover · 2 beneficiaries','Active'],
    ['Family funeral plan','Arranged by Royal Square','R50 000 · 5 family members','Active']
  ]),
  '/area/protection':()=>area('My Protection','Vehicle, home and business cover in one view.',[
    ['Vehicle cover','Short-term insurance','2021 Toyota Corolla · CA 419 872','Active'],
    ['Home contents','Short-term insurance','R380 000 contents cover','Active'],
    ['Business assurance','Business cover','Buy-and-sell agreement · under review','Under review']
  ]),
  '/area/assets':()=>area('My Assets','Investments, shares, unit trusts and property.',[
    ['Unit trust portfolio','Investments','Balanced fund · monthly R2 500','Active'],
    ['Share portfolio','Equities','Statement available for August','Active'],
    ['Property bond','Property','12 Acacia Road, Durban','Active']
  ])
};

const navItems=[
  {path:'/', label:'Overview', icon:'⌂'},
  {path:'/documents', label:'Documents', icon:'▧'},
  {path:'/help', label:'Get help', icon:'✥'},
  {path:'/adviser', label:'Adviser', icon:'◯'}
];

function link(path,cls,inner){return `<a href="${path}" class="${cls}">${inner}</a>`}
function routeTo(path){
  history.pushState({}, '', path);
  render();
  window.scrollTo(0, 0);
}
function renderSidebar(){
  navTabs.innerHTML = navItems.map(item => `
    <button type="button" class="nav-item ${location.pathname===item.path ? 'active' : ''}" data-route="${item.path}">
      <span class="nav-icon">${item.icon}</span>
      <span>${item.label}</span>
      ${item.path==='/' ? '<span class="tab-count">4</span>' : ''}
    </button>
  `).join('');
  navTabs.querySelectorAll('.nav-item').forEach(button => {
    button.onclick = () => routeTo(button.dataset.route);
  });
  roleButtons.forEach(btn=>btn.classList.toggle('active', btn.dataset.role === 'client'));
  topbarRight.innerHTML = '<button type="button" class="topbar-btn">Secure vault</button>';
}
function setHeader(path){
  const metaMap={
    '/':{title:'Dashboard',meta:'Client overview'},
    '/documents':{title:'My Documents',meta:'Stored securely and ready when you need them'},
    '/help':{title:'Get help',meta:'Support with claims, accidents and emergencies'},
    '/adviser':{title:'Ask Royal Square',meta:'Your adviser is Thabo Nkosi'},
    '/area/family':{title:'My Family',meta:'Life cover, funeral plan and beneficiaries'},
    '/area/protection':{title:'My Protection',meta:'Vehicle, home and business cover'},
    '/area/assets':{title:'My Assets',meta:'Investments, policies and property'}
  };
  const current = metaMap[path] || metaMap['/'];
  pageTitle.textContent = current.title;
  pageMeta.textContent = current.meta;
}
function home(){return `<section class="home-head panel"><p class="eyebrow">Good day,</p><h1>Samukelisiwe</h1><p class="lede">How can Royal Square help you today?</p></section>
${link('/help','emergency','<span class="icon">♧</span><span><b>I need help now</b><small>Accident, damage, emergency or a claim</small></span><span class="arrow">›</span>')}
<section class="area-list">${link('/area/family','area-link','<span class="icon"></span><span><strong>My Family</strong><small>Life cover, funeral plan and the people you protect.</small></span><span class="arrow">›</span>')}${link('/area/protection','area-link','<span class="icon"></span><span><strong>My Protection</strong><small>Vehicle, home and business cover in one view.</small></span><span class="arrow">›</span>')}${link('/area/assets','area-link','<span class="icon"></span><span><strong>My Assets</strong><small>Investments, shares, unit trusts and property.</small></span><span class="arrow">›</span>')}</section>
<section class="doc-progress panel"><div class="row"><b>My documents</b><span>63% ready</span></div><div class="progress"><i></i></div></section><p class="privacy">Your information is only shared with your consent.</p>`}
function area(icon,title,desc,cards){return `<a href="/" class="back">Home</a><section class="asset-page-head"><h1>${icon} ${title}</h1><p>${desc}</p></section><section class="cards">${cards.map(c=>`<article class="asset-card"><h2>${c[0]}</h2><p class="provider">${c[1]}</p><span class="status ${c[3]==='Under review'?'review':''}">${c[3]}</span><p class="detail">${c[2]}</p><div class="actions"><a class="btn" href="/help">Make a request</a><a class="btn" href="/documents">View documents</a></div></article>`).join('')}</section>`}
function documents(){const section=(name,items)=>`<section class="doc-section panel"><h2>${name}</h2>${items.map(x=>`<div class="doc-row"><span>${x}</span>${x.includes('*')?'<button class="btn photo secondary">Take a photo</button>':'<span class="saved">Saved</span>'}</div>`).join('').replaceAll('*','')}</section>`;return `<section class="documents-head"><h1>My Documents</h1><p>Add a document once — Royal Square will not ask for it again.</p><div class="progress-label"><span>5 of 8 ready</span><span>63%</span></div><div class="progress"><i></i></div></section>${section('Identity',['ID document','Proof of address','Driver\'s licence*'])}${section('Insurance',['Vehicle policy schedule'])}${section('Family',['Funeral plan certificate','Beneficiary form*'])}${section('Investments',['Unit trust statement'])}${section('Property',['Property bond statement*'])}<p class="storage-note">Stored securely. Shared only when you allow it.</p>`}
function help(){const choices=[' Vehicle accident','Property damage','Financial problem',' Family emergency',' Document problem',' Something else'];return `<section class="help-head"><h1>What happened?</h1><p>Choose the closest option — we will guide you from there.</p></section><section class="help-list">${choices.map(x=>`<button class="help-choice" data-help="${x.slice(2)}"><span>${x.slice(0,2)}</span>${x.slice(3)}</button>`).join('')}</section>`}
function adviser(){const prompts=['I want to protect my family if something happens to me','What is a unit trust?','I want to add a beneficiary','I want to update my vehicle details'];return `<section class="adviser-head"><h1>Ask Royal Square</h1><p>Your adviser is Thabo Nkosi. Tell us what you need in your own words.</p></section><section class="adviser-prompts">${prompts.map(p=>`<button class="adviser-prompt" data-prompt="${p}">${p}</button>`).join('')}</section><div class="question-box"><textarea id="question" aria-label="Type your question" placeholder="Type your question…"></textarea></div><button id="send" class="send" disabled>Send to Royal Square</button>`}
function toast(msg){const el=document.querySelector('#toast');el.textContent=msg;el.classList.add('show');setTimeout(()=>el.classList.remove('show'),2800)}
function render(){
  const route = routes[location.pathname] || home;
  app.innerHTML = route();
  setHeader(location.pathname);
  renderSidebar();
  document.querySelectorAll('.bottom-nav a').forEach(a => a.classList.toggle('active', a.getAttribute('href') === location.pathname));
  document.querySelectorAll('.help-choice').forEach(b => b.onclick = () => toast(`We’ll help you with ${b.dataset.help.toLowerCase()}. A Royal Square adviser will be in touch.`));
  document.querySelectorAll('.photo').forEach(b => b.onclick = () => toast('Photo upload is ready to start securely.'));
  document.querySelectorAll('.adviser-prompt').forEach(b => b.onclick = () => { const q=document.querySelector('#question'); q.value=b.dataset.prompt; q.dispatchEvent(new Event('input')); });
  const q = document.querySelector('#question');
  const send = document.querySelector('#send');
  if (q && send) {
    q.oninput = () => send.disabled = !q.value.trim();
    send.onclick = () => {
      toast('Your question has been sent to Royal Square.');
      q.value = '';
      send.disabled = true;
    };
  }
}
document.addEventListener('click',e=>{
  const a=e.target.closest('a[href]');
  if(a&&a.origin===location.origin){
    e.preventDefault();
    history.pushState({},'',a.getAttribute('href'));
    render();
    window.scrollTo(0,0);
  }
});
window.addEventListener('popstate',render);
render();
