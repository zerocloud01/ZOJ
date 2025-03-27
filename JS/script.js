let img_slide = null;
const N = 3;
document.addEventListener("DOMContentLoaded", () => {
    fetch('../head/head.html') // 页头
        .then(response => response.text())
        .then(html => {
            document.getElementById('head').innerHTML = html;
        });

    fetch('../sider/sider.html') // 侧栏
        .then(response => response.text())
        .then(html => {
            document.getElementById('sider').innerHTML = html;
        });
    
    let idx = 0, timer;
    const track = document.querySelector('.slider-track');
    const l_btn = document.querySelector(".l-btn");
    const r_btn = document.querySelector(".r-btn");
    
    // 预加载所有图片
    function preloadImages() {
        document.querySelectorAll('.slide-img').forEach((img, i) => {
            img.style.backgroundImage = `url("../home/img${i+1}.jpg")`;
        });
    }
    preloadImages();
    
    function change(op=1) {
        clearInterval(timer);
        idx = (idx + op + N) % N; 
        track.style.transform = `translateX(-${idx * 33.3333}%)`; // 滑动效果
        startTimer();
    }
    
    function startTimer() {
        timer = setInterval(() => {
            change(1);
        }, 10000);
    }
    
    l_btn.addEventListener("click", () => {
        change(-1); // 向左滑动
    });
    
    r_btn.addEventListener("click", () => {
        change(1); // 向右滑动
    });

    // 鼠标悬停暂停
    const slider = document.querySelector('.slide-images');
    slider.addEventListener("mouseenter", () => clearInterval(timer));
    slider.addEventListener("mouseleave", startTimer);

    startTimer(); // 初始化自动轮播
});