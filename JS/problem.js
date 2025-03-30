if (document.querySelector('#problemList')) {
  
    // 异步加载题目列表
    async function loadProblemList() {
        try {
            
            const response = await fetch('./problems/');// 需要放在 HTML文件夹下
            const text = await response.text();
            const parser = new DOMParser();
            const doc = parser.parseFromString(text, 'text/html');
            const links = [...doc.querySelectorAll('a')]
                // 筛选出以.md结尾的链接
                .filter(a => a.href.endsWith('.md'))
                
                .map(a => {
                    const filename = a.href.split('/').pop();
                    return {
                        id: filename.split('.')[0],  // 提取文件名作为ID
                        title: filename.replace('.md', ''),  // 去除扩展名作为标题
                        path: `./problems/${filename}`  // 完整文件路径
                    };
                });
  
            renderProblemList(links);
           
        } catch (error) {
            console.error('加载题目列表失败:', error);
        }
    }
  
    // 渲染题目列表函数
    function renderProblemList(problems) {
        const container = document.getElementById('problemList');
        // 生成题目卡片HTML
        container.innerHTML = problems.map(prob => `
            <div class="problem-card" 
                 data-id="${prob.id}" 
                 onclick="window.location='problem.html?id=${prob.id}'">
                <div class="problem-id">${prob.id}</div>  <!-- 显示题目ID -->
            </div>
        `).join('');  
    }
  
    loadProblemList();
  }
  
  // 题目详情页
  if (document.querySelector('#problemContent')) {
    
    async function loadProblemDetail() {
        const urlParams = new URLSearchParams(window.location.search);
        const problemId = urlParams.get('id');
        
        if (!problemId) {
            window.location.href = 'all_problem.html';
            return;
        }
  
        try {
            const response = await fetch(`./problems/${problemId}.md`);
            if (!response.ok) throw new Error('题目不存在');
            
            const mdContent = await response.text();
            document.getElementById('problemTitle').textContent = problemId;
            document.getElementById('problemContent').innerHTML = marked.parse(mdContent);
        } catch (error) {
            document.getElementById('problemContent').innerHTML = `
                <div class="error">${error.message}</div>
            `;
        }
    }
  
    // 页面加载时自动执行
    loadProblemDetail();
  }