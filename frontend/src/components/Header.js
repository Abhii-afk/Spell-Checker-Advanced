import React from 'react';
import styled from 'styled-components';
import { FiEdit3, FiCheck } from 'react-icons/fi';

const HeaderContainer = styled.header`
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 30px 40px;
  text-align: center;
`;

const Title = styled.h1`
  font-size: 2.5rem;
  font-weight: 700;
  margin-bottom: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 15px;

  @media (max-width: 768px) {
    font-size: 2rem;
  }
`;

const Subtitle = styled.p`
  font-size: 1.1rem;
  opacity: 0.9;
  font-weight: 300;
`;

const IconWrapper = styled.div`
  display: flex;
  align-items: center;
  gap: 5px;
`;

function Header() {
  return (
    <HeaderContainer>
      <Title>
        <IconWrapper>
          <FiEdit3 size={40} />
          <FiCheck size={30} />
        </IconWrapper>
        Advanced Spell Checker
      </Title>
      <Subtitle>
        Intelligent spell checking with smart suggestions powered by C algorithms
      </Subtitle>
    </HeaderContainer>
  );
}

export default Header;